BIN = tedomari
SOURCE_DIRS = ui ui/sdl ui/sdl/cairo game
THIS_CXXFLAGS = `pkg-config --cflags pangocairo` \
	-Wall -W -Wno-format -Woverloaded-virtual -Werror -O0 -g3
THIS_CFLAGS = -Wall -W -Wno-format -O0 -g3
# The following flag makes debugging much easier, but it needs to be removed if
# tedomari is to be installed
THIS_LDFLAGS = -no-install
LIB_DEP_NAMES = libsakusen libsakusen-comms libsakusen-resources libsakusen-client liboptimal
LIBS = -lstdc++ -lreadline -lunicode -lSDL `pkg-config --libs pangocairo`
INCLUDE =

include ../Makefile.common

