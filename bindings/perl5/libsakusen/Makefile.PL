#!/usr/bin/perl -w

use ExtUtils::MakeMaker;

my $CC = $ENV{'CXX'} || 'g++';
my $LD = '$(CC)';
my $LDDLFLAGS = $ENV{'LDDLFLAGS'} || '';
#$libtool = 'libtool';
my $top_srcdir = $ENV{'top_srcdir'} || die;
my $top_builddir = $ENV{'top_builddir'} || die;
my $this_srcdir = $ENV{'this_srcdir'} || die;

# note the order of these two assignments
#$LD = "$libtool --tag=CXX --mode=link $CC -module -avoid-version";
#$CC = "$libtool --mode=compile --tag=CXX $CC";

WriteMakefile(
  NAME => 'Sakusen',
  VERSION_FROM => 'Sakusen.pm',
  AUTHOR => 'Vapourware Marketing Board <futur@istic.org>',
  C => ['Sakusen.cpp'],
  CC => $CC,
  LD => $LD,
  DEFINE => '-DHAVE_BOOL',
  INC => '-I'.$top_srcdir.' '.$ENV{'BUILD_CPPFLAGS'},
  OBJECT => 'Sakusen.o',
  FIRST_MAKEFILE => 'Makefile.PL.out',
  #LDLOADLIBS => '../../../libsakusen/.libs/libsakusen.so',
  #LIBS => $top_builddir . 'libsakusen/.libs/libsakusen.so',
  test => { TESTS => $this_srcdir.'t/*.t' },
  LDDLFLAGS => $LDDLFLAGS.' '.$top_builddir.'sakusen/.libs/libsakusen.so -shared -Wl,--rpath -Wl,'.`cd ../../../sakusen/.libs/ && pwd`,
);
