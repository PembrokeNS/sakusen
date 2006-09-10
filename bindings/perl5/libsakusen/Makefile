PERL5 ?= perl
SWIG ?= swig
PREFIX ?= /usr/local
PERL5ARCH := $(shell $(PERL5) -MConfig -e 'print $$Config{archlib}')
INCLUDE := -I$(PERL5ARCH)/CORE -I../../../libsakusen

all: sakusen.la

test: all
	$(MAKE) -C t test

clean:
	-rm -rf *.o *.so *.pm *.cpp

# This command will compile a .so file that works, and is probably suitable
# for testing, but it MUST NOT be installed.  To compile one suitable for
# installation the --rpath here specified should be changed to $(PREFIX)/lib
#
# I think libtool should be usable except for the requirement that the name
# starts 'lib'.  It seems a little silly to be stopped by something so trivial.
sakusen.la: Makefile sakusen.o
	$(CXX) -shared -o sakusen.so sakusen.o ../../../libsakusen/.libs/libsakusen.so -Wl,--rpath -Wl,`cd ../../../libsakusen/.libs/ && pwd`

# -DHAS_BOOL was suggested as an alternative to -Dbool=char, and it seems a
# less silly option.  It also seems to work.
sakusen.o: sakusen.cpp
	$(CXX) -c sakusen.cpp $(INCLUDE) -o sakusen.o -DHAS_BOOL -Wall -fPIC

sakusen.cpp: ../../libsakusen.i
	$(SWIG) -perl5 -c++ -o sakusen.cpp -I../../../libsakusen ../../libsakusen.i

.PHONY: all test clean

