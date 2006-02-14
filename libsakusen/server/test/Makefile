BIN = libsakusen-server-test
LIBS = -lrt
LIB_DEP_NAMES = libsakusen libsakusen-server
THIS_CXXFLAGS = -Wall -W -Werror -O0 -g3
THIS_LDFLAGS = -no-install

include ../../../Makefile.common

test: $(BIN)
	./$(BIN)

.PHONY: test

