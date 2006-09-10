PERL5 ?= perl
SWIG ?= swig
PREFIX ?= /usr/local
PERL5ARCH := $(shell $(PERL5) -MConfig -e 'print $$Config{archlib}')
INCLUDE := -I$(PERL5ARCH)/CORE -I../../../libsakusen

all: sakusen.cpp

test: all
	$(MAKE) -C t test

clean:
	-rm -rf *.o *.so *.pm *.cpp *.lo *.la

sakusen.la: sakusen.o
	libtool --mode=link --tag=CXX $(CXX) -module -o $@ -rpath $(PREFIX)/lib \
		sakusen.lo ../../../libsakusen/libsakusen.la

# -DHAS_BOOL was suggested as an alternative to -Dbool=char, and it seems a
# less silly option.  It also seems to work.
sakusen.o: sakusen.cpp
	libtool --mode=compile --tag=CXX $(CXX) -c $(INCLUDE) -o sakusen.o \
		-DHAS_BOOL -Wall sakusen.cpp

sakusen.cpp: ../../libsakusen.i
	$(SWIG) -perl5 -c++ -o sakusen.cpp -I../../../libsakusen ../../libsakusen.i

.PHONY: all test clean

