#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <lzma.h>
#include <erl_nif.h>

typedef enum
{
    EX_XZ_MODE_DECOMPRESS,
    EX_XZ_MODE_COMPRESS
} ex_xz_mode;

int ex_xz_write_data(uint8_t* input, size_t input_size, ErlNifBinary* output, size_t* output_offset)
{
    if(input_size > 0)
    {
        if(output->size == 0)
        {
            if(!enif_alloc_binary(input_size, output))
            {
                return LZMA_MEM_ERROR;
            }
        }
        else
        {
            if(!enif_realloc_binary(output, output->size + input_size))
            {
                enif_release_binary(output);
                return LZMA_MEM_ERROR;
            }
        }

        memcpy(&(output->data[*output_offset]), input, input_size);
        *output_offset += input_size;
    }

    return LZMA_OK;
}

static ERL_NIF_TERM ex_xz_make_error(ErlNifEnv* env, int code)
{
    switch(code)
    {
        case LZMA_MEM_ERROR:
            return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "memory_error"));
        case LZMA_DATA_ERROR:
            return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "data_error"));
        case LZMA_FORMAT_ERROR:
            return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "format_error"));
        case LZMA_BUF_ERROR:
            return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "buffer_error"));
        case LZMA_PROG_ERROR:
            return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "program_error"));
        case LZMA_MEMLIMIT_ERROR:
            return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "memory_limit_error"));
        default:
            return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "lzma_error"));
    }
}

static ERL_NIF_TERM ex_xz_make_result(ErlNifEnv* env, uint8_t* data, size_t data_size)
{
    ErlNifBinary binary;

    if(!enif_alloc_binary(data_size, &binary))
    {
        return ex_xz_make_error(env, LZMA_MEM_ERROR);
    }

    memcpy(binary.data, data, data_size);

    return enif_make_tuple2(env, enif_make_atom(env, "ok"), enif_make_binary(env, &binary));
}

static ERL_NIF_TERM ex_xz_run(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[], int mode)
{
    ErlNifBinary input;

    if(!enif_inspect_iolist_as_binary(env, argv[0], &input))
    {
        return enif_make_badarg(env);
    }

    int ret             = LZMA_OK;
    lzma_stream stream  = LZMA_STREAM_INIT;

    if(mode == EX_XZ_MODE_COMPRESS)
    {
        ret = lzma_easy_encoder(&stream, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
    }
    else
    {
        ret = lzma_auto_decoder(&stream, UINT64_MAX, 0);
    }

    if(ret != LZMA_OK)
    {
        return ex_xz_make_error(env, ret);
    }

    uint8_t input_buffer[BUFSIZ];
    uint8_t output_buffer[BUFSIZ];

    ErlNifBinary output     = { 0, NULL };
    size_t input_offset     = 0;
    size_t output_offset    = 0;
    size_t write_size       = 0;
    int write_result        = LZMA_OK;
    lzma_action action      = LZMA_RUN;
    stream.next_in          = NULL;
    stream.avail_in         = 0;
    stream.next_out         = output_buffer;
    stream.avail_out        = sizeof(output_buffer);

    while(true)
    {
        if(stream.avail_in == 0 && input_offset < input.size)
        {
            stream.next_in      = input_buffer;
            stream.avail_in     = (input.size - input_offset) > sizeof(input_buffer) ? sizeof(input_buffer) : (input.size - input_offset);

            memcpy(input_buffer, &(input.data[input_offset]), stream.avail_in);

            input_offset += stream.avail_in;

            if(input_offset == input.size)
            {
                action = LZMA_FINISH;
            }
        }
        
        ret = lzma_code(&stream, action);

        if(stream.avail_out == 0 || ret == LZMA_STREAM_END)
        {
            write_size      = sizeof(output_buffer) - stream.avail_out;
            write_result    = ex_xz_write_data(output_buffer, write_size, &output, &output_offset);
            
            if(write_result != LZMA_OK)
            {
                if(output.size > 0)
                {
                    enif_release_binary(&output);
                }

                return ex_xz_make_error(env, ret);
            }

            stream.next_out     = output_buffer;
            stream.avail_out    = sizeof(output_buffer);
        }

        if(ret != LZMA_OK)
        {
            if(ret == LZMA_STREAM_END)
            {
                break;
            }
            if(output.size > 0)
            {
                enif_release_binary(&output);
            }

            return ex_xz_make_error(env, ret);
        }
    }

    return enif_make_tuple2(env, enif_make_atom(env, "ok"), enif_make_binary(env, &output));
}

static ERL_NIF_TERM ex_xz_decompress(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    return ex_xz_run(env, argc, argv, EX_XZ_MODE_DECOMPRESS);
}

static ERL_NIF_TERM ex_xz_compress(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    return ex_xz_run(env, argc, argv, EX_XZ_MODE_COMPRESS);
}


static ErlNifFunc functions[] = {
    {"decompress",  1, ex_xz_decompress,    0},
    {"compress",    1, ex_xz_compress,      0}
};

static int load(ErlNifEnv *env, void **priv, ERL_NIF_TERM info) 
{
    return 0;
}

static int reload(ErlNifEnv *env, void **priv, ERL_NIF_TERM info) 
{
    return 0;
}

static int upgrade(ErlNifEnv *env, void **priv, void **old_priv, ERL_NIF_TERM info) 
{
    return load(env, priv, info);
}

static void unload(ErlNifEnv *env, void *priv) 
{}

ERL_NIF_INIT(Elixir.XZ, functions, &load, &reload, &upgrade, &unload)