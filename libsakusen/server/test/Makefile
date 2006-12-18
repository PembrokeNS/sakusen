BIN = libsakusen-server-test
LIBS = -lrt
LIB_DEP_NAMES = libltdl_hacked libsakusen libsakusen-server libsakusen-comms libsakusen-resources
THIS_CXXFLAGS = #-pg
THIS_LDFLAGS = -no-install #-pg

include ../../../Makefile.common

test: $(BIN)
	./$(BIN)

.PHONY: test

