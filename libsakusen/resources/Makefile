BIN = libsakusen-resources.la
SUBDIRS = test
LIB_DEP_NAMES = libltdl_hacked libsakusen libsakusen-comms
LIBS = -lpcrecpp

include ../../Makefile.common

test: all
	$(MAKE) -C test test

.PHONY: test

