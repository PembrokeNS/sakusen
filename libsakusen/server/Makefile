SUBDIRS = test
BIN = libsakusen-server.la
LIBS = -lstdc++ -lltdl
LIB_DEP_NAMES = libsakusen
INCLUDE =

include ../../Makefile.common

test: all
	$(MAKE) -C test $@

.PHONY: test

