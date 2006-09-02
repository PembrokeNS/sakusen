BIN = libsakusen-server-test
LIBS = -lrt
LIB_DEP_NAMES = libsakusen libsakusen-server libsakusen-comms libsakusen-resources
THIS_CXXFLAGS = -Wall -W -Werror -O0 -g3 #-pg
THIS_LDFLAGS = -no-install #-pg

include ../../../Makefile.common

test: $(BIN)
	./$(BIN)

.PHONY: test

