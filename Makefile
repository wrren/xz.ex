CFLAGS ?= -O2 -Wall -Wextra -Wno-unused-parameter -I$(ERL_EI_INCLUDE_DIR)

XZ_SOURCES=$(wildcard c_src/*.c)

ifneq ($(OS),Windows_NT)
	CFLAGS += -fPIC
	ifeq ($(shell uname),Darwin)
		LDFLAGS += -dynamiclib -undefined dynamic_lookup -L/opt/homebrew/lib
		CFLAGS += -I/opt/homebrew/include
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