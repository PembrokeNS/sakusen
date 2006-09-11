#!/usr/bin/perl -Tw

use Test::More;
BEGIN {use_ok('Sakusen') or BAIL_OUT("module won't load");}

plan tests => 5;

$s = Sakusen::SPoint32->new(1, 2, 3);
$d = Sakusen::SPoint32->new(3, 5, 12);
$q = Sakusen::Quadratic->new($s, $d, 17);
isa_ok($q, 'Sakusen::Quadratic');
SKIP: {
  skip 'need to arrange for a World to be initted', 4;

  $p = $q->evaluate(19);
  isa_ok($p, 'Sakusen::SPoint32', 'evaluate() returns a Point');
  # assume g is 10
  is($p->{x}, 7, ' x');
  is($p->{y}, 12, ' y');
  is($p->{z}, -13, ' z');
}
