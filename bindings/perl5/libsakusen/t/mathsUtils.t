#!/usr/bin/perl

use Test::More;
BEGIN {use_ok('Sakusen') or BAIL_OUT("module won't load");}

plan tests => 3+3*5*5*5;

my $result = Sakusen::mathsUtils_solveQuadratic(0, 0, 0);

isa_ok($result, 'Sakusen::tupleDoubleDouble');
ok(defined $result->get_head(), "Can get first of pair");
ok(defined $result->get_tail()->get_head(), "Can get second of pair");

my @values_to_try = ( -2, -1, 0, 1, 1E100 );

foreach $a (@values_to_try) {
  foreach $b (@values_to_try) {
    foreach $c (@values_to_try) {
      $result = Sakusen::mathsUtils_solveQuadratic($a, $b, $c);
      my $root1 = $result->get_head();
      my $root2 = $result->get_tail()->get_head();
      ok(!($root2 < $root1), "roots in correct order");
      if ($a == 0) {
        # Test for NaN
        ok($root2 != $root2, "a=0 gives second root NaN");
        if ($b == 0) {
          ok($root1 != $root1, "a=b=0 gives first root NaN");
        } else {
          is($b*$root1+$c, 0, "root1 root of linear eqn");
        }
      } else {
        my $q = "$a x^2 + $b x + $c";
        if ($root1 != $root1) {
          ok($root2 != $root2, "when root1 NaN, so is root2 (of $q)");
          ok($b*$b < 4*$a*$c, "discriminant negative when no roots");
        } else {
          foreach $root ($root1, $root2) {
            my $eps = $root * 1e-10;
            my $root_plus = $root+$eps;
            my $root_minus = $root-$eps;
            my $q_of_root_plus = $a*$root_plus*$root_plus+$b*$root_plus+$c;
            my $q_of_root_minus = $a*$root_minus*$root_minus+$b*$root_minus+$c;
            # Check that these two values have different signs
            ok($q_of_root_plus*$q_of_root_minus <= 0,
              "root1 ($root1) root of $q");
          }
        }
      }
    }
  }
}

