PERL5 ?= perl
SWIG ?= swig
PREFIX ?= /usr/local
PERL5ARCH := $(shell $(PERL5) -MConfig -e 'print $$Config{archlib}')
INCLUDE := -I$(PERL5ARCH)/CORE -I../../../libsakusen

all config static dynamic test linkext manifest blibdirs realclean disttest distdir: Makefile
	$(MAKE) -f $< $@

clean: Makefile
	$(MAKE) -f $< $@
	rm -rf *.lo *.la *.cpp *.pm Makefile Makefile.old

Makefile: Makefile.PL Sakusen.cpp
	./$<

Sakusen.la: Sakusen.o
	libtool --mode=link --tag=CXX $(CXX) -module -o $@ -rpath $(PREFIX)/lib \
		Sakusen.lo ../../../libsakusen/libsakusen.la

# -DHAS_BOOL was suggested as an alternative to -Dbool=char, and it seems a
# less silly option.  It also seems to work.
Sakusen.o: Sakusen.cpp
	libtool --mode=compile --tag=CXX $(CXX) -c $(INCLUDE) -o $@ \
		-DHAS_BOOL -Wall $<

Sakusen.cpp: ../../libsakusen.i
	$(SWIG) -perl5 -c++ -o $@ -module Sakusen -I../../../libsakusen $<

.PHONY: all config static dynamic test linkext manifest blibdirs clean realclean disttest distdir

.DELETE_ON_ERROR:

