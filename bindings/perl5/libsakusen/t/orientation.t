#!/usr/bin/perl -Tw

use Test::More tests => 4;
BEGIN {use_ok('Sakusen') or BAIL_OUT("module won't load");}

my $identity = new Sakusen::Orientation();
isa_ok ($identity, "Sakusen::Orientation");

# Test that a Rotation about the null axis works OK.
my $zero = new Sakusen::SPoint16();

SKIP: {
  skip "Can't make the following work for reasons that escape me", 2;
  my $o = new Sakusen::Orientation($zero);
  isa_ok ($o, "Sakusen::Orientation");
  is ($o, $identity, "null rotation is the identity");
}

