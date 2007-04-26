#!/bin/sh

# Script to check for existence of a few executables, and disable components as
# necessary.  Note that this is *not* trying to aid portability.  Reinventing
# the configure script is a task for another day...

# The shell-scripting is probably not as portable as it might be, either.

TARGET=config.mk

function check_for_prog
{
  prog="$1"
  if which "${prog}" > /dev/null 2>&1
  then
    eval ${prog}_exists=yes
  else
    eval ${prog}_exists=no
    printf "Couldn't find program '%s'\n" "${prog}"
  fi
}

function check_for_lib
{
  lib="$1"
  if printf 'int main() { return 0; }\n' | gcc -x c -o /dev/null -l${lib} -
  then
    eval ${lib}_exists=yes
  else
    eval ${lib}_exists=no
    printf "Couldn't find library '%s'\n" "${lib}"
  fi
}

function enable_component
{
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
      OUT_CFLAGS="${OUT_CFLAGS} -DDISABLE_${component}=1"
      printf "Disabling component %s because '%s' wasn't found\n" \
        "${component}" "$1"
      return 1
    elif test "${prog_found}" != yes
    then
      printf "Internal script error: program or library '$1' has not been tested for\n" >&2
      exit 1
    fi
  done

  printf "ENABLE_%s=yes\n" ${component} >> "${TARGET}"
  #OUT_CFLAGS="${OUT_CFLAGS} -DENABLE_${component}=1"
  printf "Enabling component %s\n" "${component}"
  return 0
}

printf "# This file autogenerated by $0\n" > "${TARGET}"

check_for_prog swig
check_for_prog perl
check_for_prog mcs
#check_for_prog something_that_doesnt_exist
check_for_lib readline
check_for_lib SDL
check_for_lib cairo
check_for_lib unicode
check_for_lib pcrecpp
check_for_lib mhash
#check_for_lib something_that_doesnt_exist

if ! enable_component EVERYTHING pcrecpp mhash
then
  printf 'Sorry, that component is critical.  Aborting.\n'
  rm "${TARGET}"
  exit 1
fi

enable_component BINDINGS swig
enable_component PERL_BINDINGS swig perl
enable_component DOTNET_BINDINGS swig mcs
enable_component UNIX_SOCKETS
enable_component READLINE readline
enable_component SDL SDL
enable_component CAIRO cairo
enable_component CONVERSION unicode
#enable_component IMPOSSIBLE_COMPONENT something_that_doesnt_exist

printf "BUILD_CFLAGS =%s\n" "${OUT_CFLAGS}" >> "${TARGET}"
printf "BUILD_CXXFLAGS =%s\n" "${OUT_CFLAGS}" >> "${TARGET}"
printf "BUILD_ARCH=%s\n" $(uname -m) >> "${TARGET}"
