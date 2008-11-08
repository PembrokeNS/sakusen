# Common definitions for icc-based configurations

CXX=icc
CC=icc

# Override the flags which otherwise provide gcc-specific warning options
GLOBAL_CXXFLAGS := -Werror -Wall -Wbrief \
	-wd981 -wd1418 -wd810 -wd383 -wd444 -wd10120 -wd1572 -wd1419 -wd593
# Most of the warnings here disabled are silly, but I'd like to keep 593 except
# for the use of FD_ZERO in tedomari::AsynchronousIOHandler

