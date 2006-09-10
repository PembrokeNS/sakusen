PERL5 ?= perl

all:

# Tests only seem to work when the parent directory is the current directory.
# I suspect that this is because otherwise they can't find sakusen.pm.
# Possibly altering the "use lib" line in each script would fix that, or
# altering some environment variable
test:
	cd .. && \
	for t in $(wildcard *.t); \
	do \
		$(PERL5) -Tw t/$$t || exit 1; \
	done

