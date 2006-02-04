SUBDIRS = test comms resources client server
BIN = libsakusen.la
LIBS = -lstdc++ -lm -lmhash
THIS_CXXFLAGS = -Wall -W -Wold-style-cast -Wno-format -Woverloaded-virtual -Werror -O0 -g3

include ../Makefile.common

test: all
	$(MAKE) -C test $@
	$(MAKE) -C resources $@

.PHONY: test

