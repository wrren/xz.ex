ERLANG_PATH ?= $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
CFLAGS ?= -O2 -Wall -Wextra -Wno-unused-parameter -I$(ERLANG_PATH)

XZ_SOURCES=$(wildcard c_src/*.c)

ifneq ($(OS),Windows_NT)
	CFLAGS += -fPIC
	ifeq ($(shell uname),Darwin)
		LDFLAGS += -dynamiclib -undefined dynamic_lookup
	else
		LDFLAGS += -Wl,--no-as-needed
	endif
endif

LDFLAGS += -llzma

.PHONY: clean

all: priv/nif.so

priv/nif.so: $(XZ_SOURCES)
	@mkdir -p priv
	$(CC) $(CFLAGS) -shared -w $(LDFLAGS) -o $@ $(XZ_SOURCES)

clean:
	$(RM) priv/*