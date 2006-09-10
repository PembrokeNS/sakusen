%module sakusen
%naturalvar;
%include "std_string.i"
%include "stdint.i"

#define LIBSAKUSEN_API

%import "libsakusen-global.h"
%include "angle.h"
%include "revision.h"

#ifdef SWIGPERL
%perlcode %{
$VERSION = $sakusen::REVISION;
%}
#endif

%include "point.h"
%{
#include "point.h"
%}
namespace sakusen {
  %extend Point {
    %template(Point) Point<T>;
    %template("operator+") operator+<T>;
    %template("operator-") operator-<T>;
    %template("operator==") operator==<T>;
    %template("operator!=") operator!=<T>;
    %template("operator<=") operator<=<T>;
    %template("operator>=") operator>=<T>;
  }
  %template(SPoint16) Point<sint16>;
  %template(SPoint32) Point<sint32>;
  %template(SPoint64) Point<sint64>;
}

%include "ray.h"
%{
#include "ray.h"
%}

%include "quadratic.h"
%{
#include "quadratic.h"
%}
/* temporary */
%import "iarchive.h"
%import "oarchive.h"

%include "rectangle.h"
%{
#include "rectangle.h"
%}
%template(SRectangle16) sakusen::Rectangle<sint16>;

%include "box.h"
%{
#include "box.h"
%}
%template(SBox16) sakusen::Box<sint16>;
