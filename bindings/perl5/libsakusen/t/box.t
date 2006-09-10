#!/usr/bin/perl -Tw

use lib '..';
use Test::More;
BEGIN {use_ok('sakusen') or BAIL_OUT("module won't load");}

plan tests => 38;

$b1 = sakusen::SBox16->new;
isa_ok($b1, 'sakusen::SBox16');
ok($b1->isValid(), 'default Box->isValid()');
ok($b1->isEmpty(), 'default Box->isEmpty()');

$p1 = sakusen::SPoint16->new(-70, 4, 18);
$p2 = sakusen::SPoint16->new(20, 103, 27);
$b2 = sakusen::SBox16->new($p1, $p2);
isa_ok($b2, 'sakusen::SBox16');
ok($b2->isValid(), 'valid Box->isValid()');

$p3 = sakusen::SPoint16->new(0, 20, 10);
$b3 = sakusen::SBox16->new($p1, $p3);
ok(!$b3->isValid(), 'invalid Box->isValid() returns false');

ok($p1 == $b2->getMin(), 'getMin()');
ok($p2 == $b2->getMax(), 'getMax()');

$b4 = sakusen::SBox16->new($p1, $p2);
ok($b2 == $b4, 'equal Boxes are ==');
ok($b2 != $b1, 'unequal Boxes are !=');

$empty = sakusen::SBox16->new($p2, $p2);
ok($empty->isEmpty(), 'empty Box->isEmpty()');
ok(!$b2->isEmpty(), 'non-empty Box->isEmpty() returns false');

ok($b2->contains($p1), 'lower-left corner is inside the Box');
ok(!$b2->contains($p2), 'upper-right corner is outside the Box');

$r = $b2->rectangle();
is($r->{minx}, -70, 'rectangle() minx');
is($r->{miny}, 4, 'rectangle() miny');
is($r->{maxx}, 20, 'rectangle() maxx');
is($r->{maxy}, 103, 'rectangle() maxy');

# intersection 
$p4 = sakusen::SPoint16->new(-50, -4, 18);
$p5 = sakusen::SPoint16->new(90, 50, 20);
$b5 = sakusen::SBox16->new($p4, $p5);

ok($b2->intersects($b5), 'intersecting Boxes intersect');
ok(!$b5->intersects($empty), "empty Box doesn't intersect");
ok(!$empty->intersects($empty), ' not even with itself');
$i1 = $b2->intersection($b5);
$i2 = $b5->intersection($b2);
ok($i1 == $i2, 'intersection() commutes');
$imin = $i1->getMin();
$imax = $i1->getMax();
is($imin->{x}, -50, 'isxn min x');
is($imin->{y}, 4, 'isxn min y');
is($imin->{z}, 18, 'isxn min z');
is($imax->{x}, 20, 'isxn max x');
is($imax->{y}, 50, 'isxn max y');
is($imax->{z}, 20, 'isxn max z');

$iempty = $b5->intersection($empty);
ok($iempty->isEmpty(), 'intersection with empty Box isEmpty()');

$u1 = $b2->enclosure($b5);
# union
$u2 = $b5->enclosure($b2);

ok($u1 == $u2, 'enclosure() commutes');
$umin = $u1->getMin();
$umax = $u1->getMax();
is($umin->{x}, -70, 'union min x');
is($umin->{y}, -4, 'union min y');
is($umin->{z}, 18, 'union min z');
is($umax->{x}, 90, 'union max x');
is($umax->{y}, 103, 'union max y');
is($umax->{z}, 27, 'union max z');

TODO: {
  $TODO = "enclosure() doesn't handle empty Boxes correctly";
  $u1 = $b2->enclosure($b1);
  ok($u1 == $b2, 'enclosure() with an empty Box is the same');

  $uempty = $b1->enclosure($empty);
  ok($uempty->isEmpty(), 'enclosure() of two empty Boxes isEmpty()');
}
