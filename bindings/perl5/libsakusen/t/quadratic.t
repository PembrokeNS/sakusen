#!/usr/bin/perl -Tw

use Test::More;
BEGIN {use_ok('sakusen') or BAIL_OUT("module won't load");}

plan tests => 5;

$s = sakusen::SPoint32->new(1, 2, 3);
$d = sakusen::SPoint32->new(3, 5, 12);
$q = sakusen::Quadratic->new($s, $d, 17);
isa_ok($q, 'sakusen::Quadratic');
SKIP: {
  skip 'need to arrange for a World to be initted', 4;

  $p = $q->evaluate(19);
  isa_ok($p, 'sakusen::SPoint32', 'evaluate() returns a Point');
  # assume g is 10
  is($p->{x}, 7, ' x');
  is($p->{y}, 12, ' y');
  is($p->{z}, -13, ' z');
}
