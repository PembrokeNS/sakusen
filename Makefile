top_srcdir = 
top_builddir = 
this_srcdir = 

all:

# Don't include the config file if we're trying to make it
ifneq ($(MAKECMDGOALS),config)
include config.mk
endif

include Makefile.local
include makefiles/Makefile.common

all-nonrecursive: config.mk

AUTOCONFIG = tools/autogenerate-config.sh

config.mk:
	@echo
	@echo "*** If you haven't looked at INSTALL yet, now might be a good time to"
	@echo "*** hit Ctrl+C and do so."
	@echo "*** Otherwise, best guess build will proceed in 2 seconds..."
	@echo "*** (This message will only appear if config.mk is absent)"
	@echo
	sleep 2
	$(AUTOCONFIG)

config:
	$(AUTOCONFIG)

doc:
	doxygen

.PHONY: config doc

