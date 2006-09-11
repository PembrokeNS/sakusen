#!/usr/bin/perl -Tw

use Test::More;
BEGIN {use_ok('sakusen') or BAIL_OUT("module won't load");}

@types = qw(SPoint64 SPoint32 SPoint16);
@primitives = qw(sint16 sint32 sint64 uint16 uint32 uint64 double);

# tests per templated type and per primitive type
plan tests => (33 * scalar @types) + (3 * scalar @primitives);

# test getting limits
{
  for $prim (@primitives) {
    my $min = eval "sakusen::bottom$prim()";
    my $max = eval "sakusen::top$prim()";
    ok(defined $min, "result of bottomNumber<$prim> is defined");
    ok(defined $max, "result of topNumber<$prim> is defined");
    ok($min < $max, "bottomNumber is less than topNumber of $prim");
  }
}

for $type (@types) {
  $p = eval "new sakusen::$type(1, 2, 3)";
  isa_ok ($p, "sakusen::$type") or diag $@;
  is ($p->{x}, 1, "x co-ord is set correctly by the ctr");
  is ($p->{y}, 2, "y co-ord is set correctly by the ctr");
  is ($p->{z}, 3, "z co-ord is set correctly by the ctr");

  $p->{x} = 4;
  is ($p->{x}, 4, "x co-ord can be modified");
  is ($p->{y}, 2, "y co-ord is unchanged");
  is ($p->{z}, 3, "z co-ord is unchanged");

  $q = eval "new sakusen::$type(5, 6, 7)";
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

  ok ($p <= $r, "<= seems to work");

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
    my $min = eval "sakusen::${type}::bottom()";
    my $max = eval "sakusen::${type}::top()";
    isa_ok($min, "sakusen::$type") or diag $@;
    isa_ok($max, "sakusen::$type") or diag $@;
    ok($min <= $max, 'bottom() <= top()');
  }

}
