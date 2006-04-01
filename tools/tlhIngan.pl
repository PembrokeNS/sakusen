#!/usr/bin/perl -w
#
# Script for translating Okrand-style tlhIngan Hol fragments to the
# assigned part of the Private Use Area in Unicode. It takes
# transliterated Klingon on stdin and prints Unicode Klingon on
# stdout.
# 
# The mapping is as follows:
# + U+f8d0 U+f8e0 U+f8f0
# 0    a      Q      0
# 1    b      r      1
# 2    ch     S      2
# 3    D      t      3
# 4    e     tlh     4
# 5    gh     u      5
# 6    H      v      6
# 7    I      w      7
# 8    j      y      8
# 9    l      '      9
# a    m
# b    n
# c    ng
# d    o             ,
# e    p             .
# f    q             *

# Yes, I know Klingons don't use commas or full stops, but they might
# be useful, and there are glyphs assigned. * is mapped to the
# "Klingon mummification symbol" -- the trefoil symbol you see
# everywhere. All other characters are left untouched.

binmode (STDOUT, ":utf8");
while (<>) {
  # first do the hard chars
  s/gh/\x{f8d5}/g;
  s/ch/\x{f8d2}/g;
  s/ng/\x{f8dc}/g;
  s/tlh/\x{f8e4}/g;

  # now do the single chars
  y[abDeHIjl-no-qQrStu-wy'0-9,.*]
   [\x{f8d0}\x{f8d1}\x{f8d3}\x{f8d4}\x{f8d6}-\x{f8db}\x{f8dd}-\x{f8e3}\x{f8e5}-\x{f8e9}\x{f8f0}-\x{f8f9}\x{f8fd}-\x{f8ff}];
  print;
}
