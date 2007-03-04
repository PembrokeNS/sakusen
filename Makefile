SUBDIRS = \
	liboptimal libltdl_hacked \
	libsakusen \
	data server-plugins \
	fuseki tedomari

include Makefile.common

test: all
	$(MAKE) -C libsakusen $@

doc:
	doxygen

.PHONY: test doc

