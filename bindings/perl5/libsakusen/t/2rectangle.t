#!/usr/bin/perl -Tw

use Test::More;
BEGIN {use_ok('sakusen') or BAIL_OUT("module won't load");}

plan tests => 25;

$r1 = sakusen::SRectangle32->new();
isa_ok($r1, 'sakusen::SRectangle32');
ok($r1->isEmpty(), 'default Rectangle is empty');

$r2 = sakusen::SRectangle32->new(2, 3, 10, 12);
isa_ok($r2, 'sakusen::SRectangle32');
is($r2->getMinX(), 2, 'getMinX()');
is($r2->getMinY(), 3, 'getMinY()');
is($r2->getMaxX(), 10, 'getMaxX()');
is($r2->getMaxY(), 12, 'getMaxY()');

is($r2->getWidth(), 9, 'getWidth()');
is($r2->getHeight(), 10, 'getHeight()');

$p = sakusen::SPoint32->new(5, 5, -1);
ok($r2->contains($p), 'contains() is true in an easy case');
%$p = (x => 2, y => 3, z => -1);
ok($r2->contains($p), 'rect contains() its lower-left corner');
$p->{x} = 10;
ok($r2->contains($p), ' and its right edge');
$p->{x} = 5; $p->{y} = 12;
ok($r2->contains($p), ' and its upper edge');
$p->{x} = 50;
ok(!$r2->contains($p), ' but not a point outside');

%$r1 = (minx => -5, miny => 2, maxx => 8, maxy => 15);

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
}

diag('TODO: Test some more cases for intersection.');

#union
{
  $u1 = $r1->enclosure($r2);
  $u2 = $r2->enclosure($r1);
  ok($u1 == $u2, 'enclosure() commutes');
  is($u1->{minx}, -5, 'union minx');
  is($u1->{miny}, 2, 'union miny');
  is($u1->{maxx}, 10, 'union maxx');
  is($u1->{maxy}, 15, 'union maxy');
}

diag('TODO: Test load and store once IArchive and OArchive are up and running.');
diag('TODO: Test intersecting with ICompleteUnit and ISensorReturns once bindings exist.');
