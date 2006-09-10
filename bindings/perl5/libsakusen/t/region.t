#!/usr/bin/perl -Tw

use Test::More;
BEGIN {use_ok('sakusen') or BAIL_OUT("module won't load");}

my @coords = ([5, 5, -30], [-6, 5, 0], [9, 12, 5]);

plan tests => 15 + 9 * scalar @coords;

my @points = ();
for (@coords) {
  push @points, sakusen::SPoint32->new(@$_);
  isa_ok($points[$#points-1], 'sakusen::SPoint32');
}


#
# rectangular region
#

$rect = sakusen::SRectangle32->new(0,0, 15, 10);
$rrd = sakusen::SRectangleRegionData32->new($rect);
isa_ok($rrd, 'sakusen::SRectangleRegionData32');
isa_ok($rrd, 'sakusen::SRegionData32');

$region = sakusen::SRegion32->new($rrd);
isa_ok($region, 'sakusen::SRegion32');

ok($region->contains($points[0]), "contains() point inside");
ok(!$region->contains($points[1]), "!contains() point to the left");
ok(!$region->contains($points[2]), "!contains() point to the top");

diag('TODO: Test contains(IUnitStatus*)');

# check truncateToFit
my @rtrunc = ();
for (@points) {
  push @rtrunc, $region->truncateToFit($_);
}

for (@rtrunc) {
  isa_ok($_, 'sakusen::SPoint32');
  ok($region->contains($_), 'truncateToFit()ed point is inside the rectangular Region');
}

ok($points[0] == $rtrunc[0], 'truncateToFit() has no effect on points inside');
ok($points[1] != $rtrunc[1], ' but it does on points to the left');
ok($points[2] != $rtrunc[2], ' and on points to the top');

# check getBestPosition()
$p = $region->getBestPosition();
# round towards zero
is($p->{x}, 7, 'rectangle getBestPosition() x');
is($p->{y}, 5, 'rectangle getBestPosition() y');
is($p->{z}, 0, 'rectangle getBestPosition() z is 0');

# check getBoundingRectangle()
$r = $region->getBoundingRectangle();
ok($r == $rect, 'getBoundingRectangle() for a rectangular Region returns the original Rectangle');

diag('TODO: Test store() when OArchive is bound');
#
# spherical region
#

$srd = sakusen::SSphereRegionData32->new($points[0], 6);
isa_ok($srd, 'sakusen::SSphereRegionData32');
isa_ok($srd, 'sakusen::SRegionData32');
$sregion = sakusen::SRegion32->new($srd);
isa_ok($sregion, 'sakusen::SRegion32');

SKIP: {
  skip 'need to arrange for a World to be initted', 4 * scalar @points;
  ok($sregion->contains($points[0]), "contains() centre");
  ok(!$sregion->contains($points[1]), "!contains() rank outsider");
  ok(!$sregion->contains($points[2]), "!contains() rank outsider");

  # check truncateToFit
  my @strunc = ();
  for (@points) {
    push @strunc, $sregion->truncateToFit($_);
  }

  for (@strunc) {
    isa_ok($_, 'sakusen::SPoint32');
    ok($sregion->contains($_), 'truncateToFit()ed point is inside the spherical Region');
  }

  ok($points[0] == $strunc[0], 'truncateToFit() has no effect on points inside the sphere');
  ok($points[1] != $strunc[1], ' but it does on points to the left');
  ok($points[2] != $strunc[2], ' and on points to the top');
}
# check getBestPosition()
$p = $sregion->getBestPosition();
ok($p == $points[0], 'getBestPosition() for a spherical region returns the centre of the sphere');

# check getBoundingRectangle()
$sr = $sregion->getBoundingRectangle();
is($sr->getMinX(), -1, 'getBoundingRectangle() for a spherical Region, minx');
is($sr->getMinY(), -1, 'getBoundingRectangle() for a spherical Region, miny');
is($sr->getMaxX(), 11, 'getBoundingRectangle() for a spherical Region, maxx');
is($sr->getMaxY(), 11, 'getBoundingRectangle() for a spherical Region, maxy');
