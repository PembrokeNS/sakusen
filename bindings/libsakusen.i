%module sakusen
%naturalvar;
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"

#define LIBSAKUSEN_API

%import "libsakusen-global.h"
/* tiny files */
%include "angle.h"
%include "revision.h"
%include "hitpoints.h"
%include "topology.h"
%{
#include "topology.h"
%}

#ifdef SWIGPERL
#define BINDING_NOOVERLOAD
%perlcode %{
$VERSION = 0.01;
%}
#endif

%include "point.h"
%{
#include "point.h"
%}
namespace sakusen {
#ifdef BINDING_NOOVERLOAD
%template(bottomsint16) bottomNumber<sint16>;
%template(bottomsint32) bottomNumber<sint32>;
%template(bottomsint64) bottomNumber<sint64>;
%template(topsint16) topNumber<sint16>;
%template(topsint32) topNumber<sint32>;
%template(topsint64) topNumber<sint64>;
%template(bottomuint16) bottomNumber<uint16>;
%template(bottomuint32) bottomNumber<uint32>;
%template(bottomuint64) bottomNumber<uint64>;
%template(topuint16) topNumber<uint16>;
%template(topuint32) topNumber<uint32>;
%template(topuint64) topNumber<uint64>;
%template(bottomdouble) bottomNumber<double>;
%template(topdouble) topNumber<double>;
#endif
  %extend Point {
    %template(Point) Point<T>;
    %template("operator+") operator+<T>;
    %template("operator-") operator-<T>;
    %template("operator==") operator==<T>;
    %template("operator!=") operator!=<T>;
    %template("operator<=") operator<=<T>;
    %template("operator>=") operator>=<T>;
    %template("operator<") operator<<T>;
    %template("operator>") operator><T>;
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
%template(SRectangle32) sakusen::Rectangle<sint32>;

%include "box.h"
%{
#include "box.h"
%}
%template(SBox32) sakusen::Box<sint32>;

/* in lieu of including playerid.h */
typedef uint8 PlayerID;

%include "regiondata.h"
%include "sphereregiondata.h"
%include "rectangleregiondata.h"
%include "regiontype.h"
%include "region.h"
%{
#include "regiontype.h"
#include "sphereregiondata.h"
#include "rectangleregiondata.h"
#include "regiondata.h"
#include "region.h"
#include "region-methods.h"
%}
%template(SRegionData32) sakusen::RegionData<sint32>;
%template(SRectangleRegionData32) sakusen::RectangleRegionData<sint32>;
%template(SSphereRegionData32) sakusen::SphereRegionData<sint32>;
%template(SRegion32) sakusen::Region<sint32>;

/* we shouldn't need to call any of the functions herein */
%import "perception.h"
%import "sensorreturn.h"
%import "eachsensorreturn.h"

%include "visibility.h"
#{
#include "perception.h"
#include "visibility.h"
#}

/* we shouldn't need to call any of the functions herein */
%import "playertemplate.h"
%import "ordertype.h"
