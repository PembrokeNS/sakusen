#!/usr/bin/perl -Tw

use Test::More;
BEGIN {use_ok('Sakusen') or BAIL_OUT("module won't load");}

plan tests => 36;

$empty = Sakusen::SBox32->new;
isa_ok($empty, 'Sakusen::SBox32');
ok($empty->isEmpty(), 'default Box->isEmpty()');

$p1 = Sakusen::SPoint32->new(-70, 4, 18);
$p2 = Sakusen::SPoint32->new(20, 103, 27);
$b2 = Sakusen::SBox32->new($p1, $p2);
isa_ok($b2, 'Sakusen::SBox32');

$p3 = Sakusen::SPoint32->new(0, 20, 10);
$b3 = Sakusen::SBox32->new($p1, $p3);

ok($p1 == $b2->getMin(), 'getMin()');
ok($p2 == $b2->getMax(), 'getMax()');

$b4 = Sakusen::SBox32->new($p1, $p2);
ok($b2 == $b4, 'equal Boxes are ==');
ok($b2 != $b3, 'unequal Boxes are !=');
ok($b2 != $empty, 'non-empty Box != empty Box');

ok(!$b2->isEmpty(), 'non-empty Box->isEmpty() returns false');

ok($b2->contains($p1), 'lower-left corner is inside the Box');
ok($b2->contains($p2), 'upper-right corner is inside the Box');
ok(!$b2->contains($p3), 'outsider point is outside the Box');

$r = $b2->rectangle();
is($r->{minx}, -70, 'rectangle() minx');
is($r->{miny}, 4, 'rectangle() miny');
is($r->{maxx}, 20, 'rectangle() maxx');
is($r->{maxy}, 103, 'rectangle() maxy');

# intersection 
$p4 = Sakusen::SPoint32->new(-50, -4, 18);
$p5 = Sakusen::SPoint32->new(90, 50, 20);
$b5 = Sakusen::SBox32->new($p4, $p5);

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

# union
$u1 = $b2->enclosure($b5);
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

{
  $u1 = $b2->enclosure($empty);
  ok($u1 == $b2, 'enclosure() with an empty Box is the same');

  $uempty = $empty->enclosure($empty);
  ok($uempty->isEmpty(), 'enclosure() of two empty Boxes isEmpty()');
}
