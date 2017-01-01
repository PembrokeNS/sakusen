#!/usr/bin/env bash

# Script to check for existence of a few executables, and disable components as
# necessary.  Note that this is *not* trying to aid portability.  Reinventing
# the configure script is a task for another day...

# The shell-scripting is probably not as portable as it might be, either.

TARGET=config.mk

function check_for_prog
{
  prog="$1"
  name="$prog"
  if test $# -gt 1
  then
    prog="$2"
  fi

  if which "${prog}" > /dev/null 2>&1
  then
    eval ${name}_exists=yes
  else
    eval ${name}_exists=no
    printf "Couldn't find program '%s'\n" "${prog}"
  fi
}

function check_for_lib
{
  if test $# -lt 2
  then
    printf "Internal script error: check_for_lib got $# arguments\n"
    exit 2
  fi
  # First argument is usable name
  name="$1"
  shift
  # Second argument is header name
  header="$1"
  shift
  # Subsequent arguments are things to pass to gcc (probably includes and -lfoo)
  if printf '#include <stdio.h>\n#include <%s>\nint main() { return 0; }\n' "${header}" | \
    g++ $CPPFLAGS $LDFLAGS -x c++ -o temp.out "$@" -
  then
    eval ${name}_exists=yes
  else
    eval ${name}_exists=no
    printf "Missing either library '%s' or its header '%s'\n" "${name}" "${header}"
  fi
}
function check_for_pyplusplus
{
  if python -c "import pyplusplus"
  then
    pyplusplus_exists=yes
  else
    pyplusplus_exists=no
  fi
}

function enable_component
{
  failure=0
  # First argument is component name
  component=$1
  # Subsequent arguments are programs or libraries that must exist
  while true
  do
    shift
    if test $# = 0
    then
      break
    fi

    eval prog_found=\$$1_exists

    if test "${prog_found}" = no
    then
      printf "ENABLE_%s=no\n" ${component} >> "${TARGET}"
      printf "Disabling component %s because '%s' wasn't found\n" \
        "${component}" "$1"
      failure=1
      break
    elif test "${prog_found}" != yes
    then
      printf "Internal script error: program or library '$1' has not been tested for\n" >&2
      exit 1
    fi
  done

  if test $failure = 0
  then
    printf "ENABLE_%s=yes\n" ${component} >> "${TARGET}"
    printf "Enabling component %s\n" "${component}"
  fi

  printf "ifeq (\$(ENABLE_%s),no)\n" ${component} >> "${TARGET}"
  printf '  BUILD_CPPFLAGS += -DDISABLE_%s\n' ${component} >> "${TARGET}"
  printf "endif\n\n" ${component} >> "${TARGET}"
  
  return $failure
}

printf "# This file autogenerated by %s\n" "$0" > "${TARGET}"
printf "BUILD_CPPFLAGS :=-DBOOST_FILESYSTEM_VERSION=3\n" >> "${TARGET}"

check_for_prog swig
check_for_prog perl
check_for_prog python python-config
check_for_pyplusplus
check_for_prog pyqt4 pykdeuic4
#check_for_prog something_that_doesnt_exist
check_for_lib boost boost/shared_ptr.hpp
check_for_lib boost_filesystem boost/filesystem/path.hpp -lboost_filesystem -lboost_system -DBOOST_SYSTEM_NO_DEPRECATED
check_for_lib readline readline/readline.h -lreadline
check_for_lib SDL SDL/SDL.h -lSDL
check_for_lib pangocairo pango/pangocairo.h `pkg-config pangocairo --cflags --libs`
check_for_lib icuuc unicode/ucnv.h -licuuc
check_for_lib pcrecpp pcrecpp.h -lpcrecpp
check_for_lib mhash mhash.h -lmhash
check_for_lib png png.h -lpng
check_for_lib avahiclient avahi-client/client.h -lavahi-client
# Having 2.4 here is correct: that is the API version, not the library version
check_for_lib glademm libglademm.h `pkg-config libglademm-2.4 --cflags --libs`
check_for_lib tcl tcl/tcl.h
#check_for_lib something_that_doesnt_exist some_header.h

if ! enable_component EVERYTHING pcrecpp mhash png boost boost_filesystem
then
  printf 'Sorry, that component is critical.  Aborting.\n'
  rm "${TARGET}"
  exit 1
fi

enable_component BINDINGS swig
enable_component PERL_BINDINGS swig perl
enable_component PYTHON_BINDINGS swig python
enable_component PYSTE_BINDINGS python pyplusplus
enable_component TCL_BINDINGS swig tcl
enable_component UNIX_SOCKETS
enable_component READLINE readline
enable_component SDL SDL
enable_component CAIRO pangocairo
enable_component CONVERSION icuuc
enable_component AVAHI avahiclient
enable_component KIAI pyqt4
enable_component UNIVERSE_BUILDER glademm
#enable_component IMPOSSIBLE_COMPONENT something_that_doesnt_exist

printf "BUILD_ARCH := %s\n" $(uname -m) >> "${TARGET}"
