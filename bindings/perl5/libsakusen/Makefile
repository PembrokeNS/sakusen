PERL5 ?= perl
SWIG ?= swig
PERL5ARCH := $(shell $(PERL5) -MConfig -e 'print $$Config{archlib}')
INCLUDE := -I$(PERL5ARCH)/CORE -I../../../libsakusen
include ../../../Makefile.common
all: sakusen.so

sakusen.so: sakusen.o
	g++ -shared sakusen.o \
	-Xlinker -rpath ../../../libsakusen/.libs/libsakusen.so \
	-o sakusen.so
	#libtool --mode=link --tag=CXX $(CXX) sakusen.o ../../../libsakusen/libsakusen.la -o sakusen.la -rpath $(PREFIX)/lib/libsakusen.so

sakusen.o: sakusen.cpp
	g++ -c sakusen.cpp $(INCLUDE) -o sakusen.o -Dbool=char -Wall -fPIC
	#libtool --mode=compile --tag=CXX $(CXX) -c sakusen.cpp $(INCLUDE) -o sakusen.o -Dbool=char -Wall -fPIC

sakusen.cpp: ../../libsakusen.i
	swig -perl5 -c++ -o sakusen.cpp -I../../../libsakusen ../../libsakusen.i

.PHONY: all
