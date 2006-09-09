SUBDIRS = libltdl_hacked libsakusen data liboptimal fuseki tedomari

include Makefile.common

test: all
	$(MAKE) -C libsakusen $@

doc:
	doxygen

.PHONY: test doc

