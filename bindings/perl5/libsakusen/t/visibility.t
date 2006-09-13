#!/usr/bin/perl -Tw

use Test::More;
BEGIN {use_ok('Sakusen') or BAIL_OUT("module won't load");}

plan tests => 12;

my $v = Sakusen::Visibility->new();
isa_ok($v, 'Sakusen::Visibility');

is($v->{optical}, 0, 'constructor initialises to 0: optical');
is($v->{infraRed}, 0, 'constructor initialises to 0: infraRed');
is($v->{radar}, 0, 'constructor initialises to 0: radar');
is($v->{passiveRadar}, 0, 'constructor initialises to 0: passiveRadar');
is($v->{sonar}, 0, 'constructor initialises to 0: sonar');
is($v->{passiveSonar}, 0, 'constructor initialises to 0: passiveSonar');
is($v->{seismar}, 0, 'constructor initialises to 0: seismar');

$v->{optical} = 3;
$v->{infraRed} = -1;
$v->{radar} = 2;

my $defaultsc = Sakusen::SensorCapability->new();
isa_ok($defaultsc, 'Sakusen::SensorCapability');

ok(!$defaultsc->{capable}, 'default SensorCapability is not capable');

my $sensors = Sakusen::Sensors->new();
isa_ok($sensors, 'Sakusen::Sensors');
isa_ok($sensors->{optical}, 'Sakusen::SensorCapability');
