SOURCES := $(wildcard ../universe/*.sakusensource)
SOURCE_BASES := $(notdir $(basename $(SOURCES)))
SOURCE_MODULES := $(addsuffix .sakusenmodule.la,$(SOURCE_BASES))

DIRS := $(wildcard ../universe/*.dir)
DIR_BASES := $(notdir $(basename $(DIRS)))
DIR_MAKEFRAGS := $(foreach base,$(DIR_BASES),.makefrags/$(base).makefrag)
DIR_MAKEFILES := $(addsuffix .dir-tmp/Makefile,$(DIR_BASES))
DIR_MODULES := $(addsuffix .sakusenmodule.la,$(DIR_BASES))

SOURCE_MODULES := $(filter-out $(DIR_MODULES),$(SOURCE_MODULES))

all: $(SOURCE_MODULES) $(DIR_MODULES)

include $(DIR_MAKEFRAGS)

$(SOURCE_MODULES): %.sakusenmodule.la: ../universe/%.sakusensource ../../libsakusen/server/libsakusen-server.la
	-rm -rf $*.tmp
	mkdir -p $*.tmp
	tar -xjf $< -C $*.tmp
	echo 'BIN = ../$@' > $*.tmp/Makefile
	cat Makefile.module >> $*.tmp/Makefile
	$(MAKE) -C $*.tmp ../$@
	rm -rf $*.tmp

$(DIR_MAKEFILES): %.dir-tmp/Makefile: Makefile.module
	mkdir -p $(dir $@)
	echo 'BIN = ../$*.sakusenmodule.la' > $@
	echo 'SOURCE_DIRS = ../../universe/$*.dir' >> $@
	cat Makefile.module >> $@

$(DIR_MODULES): %.sakusenmodule.la: \
		../../libsakusen/server/libsakusen-server.la %.dir-tmp/Makefile
	$(MAKE) -C $*.dir-tmp ../$@

$(DIR_MAKEFRAGS): .makefrags/%.makefrag: ../universe/%.dir
	mkdir -p $(dir $@)
	echo $@ $*.sakusenmodule.la: $(wildcard ../universe/$*.dir/*) > $@

clean:
	-rm -rf .libs *.la *.tmp

.PHONY: all clean

