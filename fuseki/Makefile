BIN = fuseki
SOURCE_DIRS = settingstree
THIS_CXXFLAGS = -Wall -W -Wold-style-cast -Wno-format -Woverloaded-virtual \
	-Werror -O0 -g3
LIB_DEP_NAMES = liboptimal libsakusen libsakusen-comms libsakusen-resources libsakusen-server
LIBS = -lstdc++ -lpcrecpp
INCLUDE =

include ../Makefile.common

doc/man/man1/fuseki.1: $(BIN)
	help2man --output=$@ --no-info \
		--name="run a server for the game sakusen" ./$<

doc: doc/man/man1/fuseki.1

.PHONY: doc

