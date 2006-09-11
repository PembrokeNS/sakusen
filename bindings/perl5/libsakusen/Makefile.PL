#!/usr/bin/perl -w

use ExtUtils::MakeMaker;

my $CC = $ENV{'CXX'} || 'g++';
my $LD = '$(CC)';
my $LDDLFLAGS = $ENV{'LDDLFLAGS'} || '';
$libtool = 'libtool';

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
  DEFINE => '-Dbool=char',
  INC => '-I../../../libsakusen',
  OBJECT => 'Sakusen.o ../../../libsakusen/.libs/libsakusen.so',
  #LDLOADLIBS => '../../../libsakusen/libsakusen.la',
  #LIBS => '../../../libsakusen/.libs/libsakusen.so',
  LDDLFLAGS => $LDDLFLAGS . '-shared -Wl,--rpath -Wl,'.`cd ../../../libsakusen/.libs/ && pwd`,
);
