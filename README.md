# XZ

Elixir NIF wrapper for the [XZ/LZMA](https://tukaani.org/xz/) data compression function library. Presents a very simple interface for 
compressing and decompressing data using default options for now.

## Examples

```elixir
iex> input = "Hello World"
"Hello World"
iex> {:ok, compressed} = XZ.compress input
{:ok,
 <<253, 55, 122, 88, 90, 0, 0, 4, 230, 214, 180, 70, 2, 0, 33, 1, 22, 0, 0, 0,
   116, 47, 229, 163, 1, 0, 10, 72, 101, 108, 108, 111, 32, 87, 111, 114, 108,
   100, 0, 0, 198, 205, 181, 199, 103, 116, 116, 62, ...>>}
iex> {:ok, decompressed} = XZ.decompress compressed
{:ok, "Hello World"}
```

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed
by adding `xz` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:xz, "~> 0.1.0"}
  ]
end
```

`xz` requires that `liblzma` be installed on your system prior to being built. `xz` does not currently support Windows.

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc)
and published on [HexDocs](https://hexdocs.pm). Once published, the docs can
be found at [https://hexdocs.pm/lzma](https://hexdocs.pm/lzma).

