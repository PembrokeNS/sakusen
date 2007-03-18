SUBDIRS = perl5 dotNET

all test:
	@for i in $(SUBDIRS); \
	do \
		$(MAKE) -C $$i $@; \
	done

.PHONY: all test

