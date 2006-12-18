SUBDIRS = comms resources server client
BIN = libsakusen.la
LIBS = -lstdc++ -lm -lmhash
INCLUDE =

include ../Makefile.common

test: all
	$(MAKE) -C resources $@
	$(MAKE) -C server $@

.PHONY: test

