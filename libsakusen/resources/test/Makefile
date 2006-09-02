BIN = libsakusen-resources-test
LIB_DEP_NAMES = libsakusen libsakusen-comms libsakusen-resources
LIBS = -lltdl
THIS_CXXFLAGS = -Wall -W -Werror -Woverloaded-virtual -O0 -g3
THIS_LDFLAGS = -no-install

include ../../../Makefile.common

test: $(BIN)
	./$(BIN)

.PHONY: test

