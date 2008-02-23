#!/usr/bin/perl -Tw

use Test::More;
BEGIN {use_ok('Sakusen') or BAIL_OUT("module won't load");}

plan tests => 13;

my $v = Sakusen::Visibility->new();
isa_ok($v, 'Sakusen::Visibility');

my $visDefault = $Sakusen::Visibility::default_;
is($visDefault, 128, 'visibility default is 128');

is($v->{optical}, $visDefault, 'constructor initialises to default: optical');
is($v->{infraRed}, $visDefault, 'constructor initialises to default: infraRed');
is($v->{radarPassive}, $visDefault, 'constructor initialises to default: radarPassive');
is($v->{radarActive}, $visDefault,'constructor initialises to default: radarActive');
is($v->{sonarPassive}, $visDefault, 'constructor initialises to default: sonarPassive');
is($v->{sonarActive}, $visDefault,'constructor initialises to default: sonarActive');
is($v->{seismar}, $visDefault, 'constructor initialises to default: seismar');

$v->{optical} = 3;
$v->{infraRed} = 255;
$v->{radarActive} = 2;

my $defaultsc = Sakusen::SensorCapability->new();
isa_ok($defaultsc, 'Sakusen::SensorCapability');

ok(!$defaultsc->{capable}, 'default SensorCapability is not capable');

my $sensors = Sakusen::Sensors->new();
isa_ok($sensors, 'Sakusen::Sensors');
isa_ok($sensors->{optical}, 'Sakusen::SensorCapability');
