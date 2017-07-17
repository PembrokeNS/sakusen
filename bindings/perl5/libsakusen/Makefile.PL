#!/usr/bin/perl -w

use ExtUtils::MakeMaker;

my $CC = $ENV{'CXX'} || 'g++';
my $LD = '$(CC)';
my $LDDLFLAGS = $ENV{'LDDLFLAGS'} || '';
my $top_srcdir = $ENV{'top_srcdir'} || die;
my $top_builddir = $ENV{'top_builddir'} || die;
my $this_srcdir = $ENV{'this_srcdir'} || die;

my $dep_lib_dir = $top_builddir.'sakusen/.libs/';

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
  MYEXTLIB => $dep_lib_dir.'libsakusen.so',
  test => { TESTS => $this_srcdir.'t/*.t' },
  dynamic_lib => {
    OTHERLDFLAGS => '-Wl,-rpath -Wl,'.`cd $dep_lib_dir && pwd`,
  },
);
