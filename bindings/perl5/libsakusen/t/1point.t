#!/usr/bin/perl -Tw

use Test::More;
BEGIN {use_ok('Sakusen') or BAIL_OUT("module won't load");}

@types = qw(SPoint64 SPoint32 SPoint16);
@primitives = qw(sint16 sint32 sint64 uint16 uint32 uint64 double);

# tests per templated type and per primitive type
plan tests => (38 * scalar @types) + (3 * scalar @primitives) + 2;

# test getting limits
{
  for $prim (@primitives) {
    my $min = eval "Sakusen::bottom$prim()";
    my $max = eval "Sakusen::top$prim()";
    ok(defined $min, "result of bottomNumber<$prim> is defined");
    ok(defined $max, "result of topNumber<$prim> is defined");
    ok($min < $max, "bottomNumber is less than topNumber of $prim");
  }
}

for $type (@types) {
  $p = eval "new Sakusen::$type(1, 2, 3)";
  isa_ok ($p, "Sakusen::$type") or diag $@;
  is ($p->{x}, 1, "x co-ord is set correctly by the ctr");
  is ($p->{y}, 2, "y co-ord is set correctly by the ctr");
  is ($p->{z}, 3, "z co-ord is set correctly by the ctr");

  $p->{x} = 4;
  is ($p->{x}, 4, "x co-ord can be modified");
  is ($p->{y}, 2, "y co-ord is unchanged");
  is ($p->{z}, 3, "z co-ord is unchanged");

  $q = eval "new Sakusen::$type(5, 6, 7)";
  is ($q->{x}, 5, "x co-ord is set correctly by the ctr");
  is ($q->{y}, 6, "y co-ord is set correctly by the ctr");
  is ($q->{z}, 7, "z co-ord is set correctly by the ctr");

  $r = $p + $q;
  is ($r->{x}, 9, "addition: x co-ord is good");
  is ($r->{y}, 8, "addition: y co-ord is good");
  is ($r->{z}, 10, "addition: z co-ord is good");

  %$p = (x => -2, y => 0, z => 2);
  is ($p->{x}, -2, "x co-ord can be set like a hash");
  is ($p->{y}, 0, "y co-ord can be set like a hash");
  is ($p->{z}, 2, "z co-ord can be set like a hash");
  $r = $q - $p;
  is ($r->{x}, 7, "subtraction: x co-ord is good");
  is ($r->{y}, 6, "subtraction: y co-ord is good");
  is ($r->{z}, 5, "subtraction: z co-ord is good");

  %$q = (x => -2, y => 0, z => 2);
  ok ($p == $q, "equal Points are ==");
  ok ($p != $r, "unequal Points are !=");

  {
    ok ($p <= $q, "equal Points are <=");
    ok ($p <= $r, 'le points are <=');
  }
  ok ($p < $r, 'le points are <');

  $q = -$p;
  is ($q->{x}, 2, "unary negate: x co-ord is good");
  is ($q->{y}, 0, "unary negate: y co-ord is good");
  is ($q->{z}, -2, "unary negate: z co-ord is good");

  # zero
  $p->zero();
  %$q = (x => 0, y => 0, z => 0);
  ok ($p == $q, "zero()");
  ok ($q->isZero, "isZero() is true for (0,0,0)");
  %$q = (x => 0, y => 0, z => 1);
  ok (!$q->isZero, "isZero() is false for (0,0,1)");

  # length
  %$p = (x => 3, y => 4, z => 0);
  is ($p->squareLength(), 25, "squareLength()");
  is ($p->length, 5, "length()");

  # test getting top and bottom points
  {
    my $min = eval "Sakusen::${type}::bottom()";
    my $max = eval "Sakusen::${type}::top()";
    isa_ok($min, "Sakusen::$type") or diag $@;
    isa_ok($max, "Sakusen::$type") or diag $@;
    ok($min <= $max, 'bottom() <= top()');
    ok($min < $max, 'bottom() < top()');
    ok(!($min >= $max), 'not bottom() >= top()');
    ok(!($min > $max), 'not bottom() > top()');
  }

}

# test that squareLength and innerProduct work when too big to fit in 32 bits
$p = new Sakusen::SPoint32(1<<24, 1<<24, 1<<24);
is ($p->squareLength(), 3*(2**48), "big squareLength()");
is ($p->innerProduct($p), 3*(2**48), "big innerProduct()");

