SWIG ?= swig
PREFIX ?= /usr/local
INCLUDE := -I../../../libsakusen

all: sakusen.la sakusen.dll

test: all
	$(MAKE) -C t test

clean:
	-rm -rf *.o *.so *.cs *.cpp *.lo *.la

sakusen.la: sakusen.o
	libtool --mode=link --tag=CXX $(CXX) -module -o $@ -rpath $(PREFIX)/lib \
		sakusen.lo ../../../libsakusen/libsakusen.la

sakusen.o: sakusen.cpp
	libtool --mode=compile --tag=CXX $(CXX) -c $(INCLUDE) -o sakusen.o \
		-Wall sakusen.cpp

sakusen.cpp: ../../libsakusen.i
	$(SWIG) -csharp -c++ -o sakusen.cpp -I../../../libsakusen ../../libsakusen.i

sakusen.dll: sakusen.cpp $(wildcard *.cs)
	mcs -checked+ -out:$@ -recurse:'*.cs' -target:library

.PHONY: all test clean

.DELETE_ON_ERROR:

