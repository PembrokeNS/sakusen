SUBDIRS = comms resources server client
BIN = libsakusen.la
LIBS = -lstdc++ -lm -lmhash
THIS_CXXFLAGS = -Wall -W -Wold-style-cast -Wno-format -Woverloaded-virtual -Werror -O0 -g3
INCLUDE =

include ../Makefile.common

test: all
	$(MAKE) -C resources $@
	$(MAKE) -C server $@

.PHONY: test

