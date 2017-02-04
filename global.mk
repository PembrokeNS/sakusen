# Add warnings and debugging, and turn off optimizations.  To get optimizations
# and such things, they should be added to BUILD_CXXFLAGS in a config.mk or to
# THIS_CXXFLAGS in a Makefile.local (depending on whether they are supposed to
# apply to a build or a binary respectively).
GLOBAL_CXXFLAGS ?= \
	-Werror -Wall -Wextra -Woverloaded-virtual -Wundef -Wpointer-arith \
	-Wwrite-strings -O0 -g3
GLOBAL_CFLAGS ?= -Werror -O0 -g3
GLOBAL_CXXPPFLAGS ?= -std=c++14

