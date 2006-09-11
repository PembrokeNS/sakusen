PERL5 ?= perl
SWIG ?= swig
PREFIX ?= /usr/local
PERL5ARCH := $(shell $(PERL5) -MConfig -e 'print $$Config{archlib}')
INCLUDE := -I$(PERL5ARCH)/CORE -I../../../libsakusen

all: Sakusen.cpp

test: all
	$(MAKE) -C t test

clean:
	-rm -rf *.o *.so *.pm *.cpp *.lo *.la

Sakusen.la: Sakusen.o
	libtool --mode=link --tag=CXX $(CXX) -module -o $@ -rpath $(PREFIX)/lib \
		Sakusen.lo ../../../libsakusen/libsakusen.la

# -DHAS_BOOL was suggested as an alternative to -Dbool=char, and it seems a
# less silly option.  It also seems to work.
Sakusen.o: Sakusen.cpp
	libtool --mode=compile --tag=CXX $(CXX) -c $(INCLUDE) -o Sakusen.o \
		-DHAS_BOOL -Wall Sakusen.cpp

Sakusen.cpp: ../../libsakusen.i
	$(SWIG) -perl5 -c++ -o Sakusen.cpp -module Sakusen -I../../../libsakusen ../../libsakusen.i

.PHONY: all test clean

