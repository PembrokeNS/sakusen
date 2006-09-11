#!/usr/bin/perl -Tw

use Test::More;
BEGIN {use_ok('Sakusen') or BAIL_OUT("module won't load");}

plan tests => 7;

$s = Sakusen::SPoint32->new(1, 2, 3);
$d = Sakusen::SPoint32->new(0, 0, -10);
SKIP: {
  skip "argumentless ctr not written", 1;
  $r1 = Sakusen::Ray->new();
  isa_ok($r1, 'Sakusen::Ray');
}
$r2 = Sakusen::Ray->new($s, $d);
isa_ok($r2, 'Sakusen::Ray');
is($r2->distance(1), 10, 'distance()');
$p = $r2->evaluate(2);
isa_ok($p, 'Sakusen::SPoint32', 'evaluate()');
is($p->{x}, 1, ' x');
is($p->{y}, 2, ' y');
is($p->{z}, -17, ' z');
