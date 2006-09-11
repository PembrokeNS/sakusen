#!/usr/bin/perl -Tw

use Test::More;
BEGIN {use_ok('Sakusen') or BAIL_OUT("module won't load");}

plan tests => 35;

my $empty = Sakusen::SRectangle32->new();
isa_ok($empty, 'Sakusen::SRectangle32');
ok($empty->isEmpty(), 'default Rectangle is empty');
my $empty2 = Sakusen::SRectangle32->new();

$r2 = Sakusen::SRectangle32->new(2,3, 10,12);
isa_ok($r2, 'Sakusen::SRectangle32');
is($r2->getMinX(), 2, 'getMinX()');
is($r2->getMinY(), 3, 'getMinY()');
is($r2->getMaxX(), 10, 'getMaxX()');
is($r2->getMaxY(), 12, 'getMaxY()');

is($r2->getWidth(), 9, 'getWidth()');
is($r2->getHeight(), 10, 'getHeight()');

$p = Sakusen::SPoint32->new(5, 5, -1);
ok($r2->contains($p), 'contains() is true in an easy case');
%$p = (x => 2, y => 3, z => -1);
ok($r2->contains($p), 'rect contains() its lower-left corner');
$p->{x} = 10;
ok($r2->contains($p), ' and its right edge');
$p->{x} = 5; $p->{y} = 12;
ok($r2->contains($p), ' and its upper edge');
$p->{x} = 50;
ok(!$r2->contains($p), ' but not a point outside');

$r1 = Sakusen::SRectangle32->new(-5,2, 8,15);

# intersection
{
  ok($r1->intersects($r2), 'intersects() is true in an easy case');
  my $i1 = $r1->intersect($r2);
  my $i2 = $r2->intersect($r1);
  ok($i1 == $i2, 'intersect() commutes');
  is($i1->{minx}, 2, 'isxn minx');
  is($i1->{miny}, 3, 'isxn miny');
  is($i1->{maxx}, 8, 'isxn maxx');
  is($i1->{maxy}, 12, 'isxn maxy');

  $i1 = $r1->intersect($r1);
  ok($i1 == $r1, 'intersect() on the same Rectangle has no effect');
}

# intersect empty rectangles
{
  ok(!$r1->intersects($empty), 'intersects() is false for empty Rectangle');
  ok(!$empty->intersects($r1), ' and vice-versa');
  ok(!$empty->intersects($empty2), ' and for two empty Rectangles');
  my $emptyi = $r1->intersect($empty);
  ok($emptyi->isEmpty(), 'intersection of non-intersecting Rectangles isEmpty()');
  my $emptyi2 = $emptyi->intersect($empty2);
  ok($emptyi2->isEmpty(), 'intersection of empty Rectangles isEmpty()');
}

#union
{
  my $u1 = $r1->enclosure($r2);
  my $u2 = $r2->enclosure($r1);
  ok($u1 == $u2, 'enclosure() commutes');
  is($u1->{minx}, -5, 'union minx');
  is($u1->{miny}, 2, 'union miny');
  is($u1->{maxx}, 10, 'union maxx');
  is($u1->{maxy}, 15, 'union maxy');

  $u1 = $r1->enclosure($r1);
  ok($u1 == $r1, 'enclosure() on the same Rectangle has no effect');
}

# union empty rectangles
{
  my $uempty = $r1->enclosure($empty);
  ok($uempty == $r1, 'enclosure() with empty Rectangle has no effect');
  $uempty = $empty->enclosure($r1);
  ok($uempty == $r1, ' and vice-versa');
  $uempty = $empty->enclosure($empty);

  $uempty = $empty->enclosure($uempty);
  ok($uempty->isEmpty(), 'enclosure() of two empty Rectangles is empty');
}
  
# access like a hash
diag('TODO: Test load and store once IArchive and OArchive are up and running.');
diag('TODO: Test intersecting with ICompleteUnit and ISensorReturns once bindings exist.');
