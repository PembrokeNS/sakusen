PERL5 ?= perl

all:

# Tests only work if they can find sakusen.pm - the use lib '..' command allows
# this from this directory
test:
	for t in $(wildcard *.t); \
	do \
		$(PERL5) -Tw $$t || exit 1; \
	done

