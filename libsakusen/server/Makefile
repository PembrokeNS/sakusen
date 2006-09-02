SUBDIRS = test
BIN = libsakusen-server.la
THIS_CXXFLAGS = -Wall -W -Wold-style-cast -Wno-format -Woverloaded-virtual -Werror -O0 -g3
LIBS = -lstdc++ -lltdl
LIB_DEP_NAMES = libsakusen
INCLUDE =

include ../../Makefile.common

test: all
	$(MAKE) -C test $@

.PHONY: test

