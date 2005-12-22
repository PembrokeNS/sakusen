BIN = libsakusen-resources.la
SUBDIRS = test
THIS_CXXFLAGS = -Wall -W -Wold-style-cast -Wno-format -Woverloaded-virtual -Werror -O0 -g3
LIB_DEP_NAMES = libsakusen libsakusen-comms

include Makefile.common

test: all
	$(MAKE) -C test test

.PHONY: test

