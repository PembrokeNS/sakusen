SUBDIRS = libsakusen libsakusen-comms libsakusen-resources fuseki tedomari

include Makefile.common

test: all
	$(MAKE) -C libsakusen $@
	$(MAKE) -C libsakusen-resources $@

