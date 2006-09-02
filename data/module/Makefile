SOURCES := $(wildcard ../universe/*.sakusensource)
BASES := $(notdir $(basename $(SOURCES)))
MODULES := $(addsuffix .sakusenmodule.la,$(BASES))

all: $(MODULES)

$(MODULES): %.sakusenmodule.la: ../universe/%.sakusensource ../../libsakusen/server/libsakusen-server.la
	-rm -rf $*.tmp
	mkdir -p $*.tmp
	tar -xjf $< -C $*.tmp
	echo 'BIN = ../$@' > $*.tmp/Makefile
	cat Makefile.module >> $*.tmp/Makefile
	$(MAKE) -C $*.tmp ../$@
	rm -rf $*.tmp

clean:
	-rm -rf .libs *.la *.tmp

.PHONY: all clean

