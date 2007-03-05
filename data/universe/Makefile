DIRS := $(wildcard *.dir)
BASES := $(basename $(DIRS))
SOURCES := $(addsuffix .sakusensource,$(BASES))
MAKEFRAGS := $(foreach base,$(BASES),.makefrags/$(base).makefrag)

all: sources

sources: $(SOURCES)

include $(MAKEFRAGS)

$(SOURCES): %.sakusensource: .makefrags/%.makefrag
	cd $*.dir && tar -cjf ../$@ $(notdir $(filter $*.dir/%,$^))

$(MAKEFRAGS): .makefrags/%.makefrag: %.dir
	mkdir -p $(dir $@)
	echo $@ $*.sakusensource: $(wildcard $*.dir/*) > $@

clean:
	-rm -rf .makefrags *.sakusensource

.PHONY: all sources clean

