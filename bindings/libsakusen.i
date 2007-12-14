%module sakusen
%naturalvar;
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"
%include "std_except.i"

#ifdef SWIGPERL
#define BINDING_NOOVERLOAD
%perlcode %{
$VERSION = 0.01;
%}

%rename(numify) *::operator uint32;
#endif

#ifdef SWIGCSHARP
/* Ignore load & store methods until Archive classes bound */
%ignore *::load;
%ignore *::loadNew;
%ignore *::store;
/* And some more ignores due to other unbound classes.  Hope to be gone in due
 * course */
%ignore sakusen::SensorCapability::SensorCapability(IArchive&);
%ignore sakusen::SensorCapability::updateReturn;
%ignore sakusen::Sensors::Sensors(IArchive&);
%ignore sakusen::Sensors::updateReturns;
%ignore sakusen::Visibility::Visibility(IArchive&);

/* Renames for wrapped operators.  Although C# does support operator
 * overloading, it doesn't work in the same way as C++'s, so for now we don't
 * try to make use of it */
%rename(Item) *::operator[];
%rename(Or) *::operator|;
%rename(Minus) *::operator-;
%rename(MinusEquals) *::operator-=;
%rename(Times) *::operator*;
%rename(TimesEquals) *::operator*=;
%rename(Over) *::operator/;
%rename(OverEquals) *::operator/=;
%rename(Plus) *::operator+;
%rename(PlusEquals) *::operator+=;
%rename(Equals) *::operator==;
%ignore *::operator!=;
%rename(Leq) *::operator<=;
%rename(Geq) *::operator>=;
%rename(Less) *::operator<;
%rename(Greater) *::operator>;
%ignore *::operator=;

%ignore operator|(const GameObject&, const GameObject&);
#endif

/* General ignores to resolve ambiguities caused by SWIG's collapsing of types
 * */

%ignore sakusen::Point::operator[](int const) const;

#define LIBSAKUSEN_API

%import "libsakusen-global.h"
%include "intmunger.h"
/* tiny files */
%include "angle.h"
%include "revision.h"
%include "gameobject.h"
%include "topology.h"
%{
#include "topology.h"
%}

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

/* temporary */
%import "iarchive.h"
%import "oarchive.h"
%import "ref.h"
%import "bounded.h"
%{
#include "isensorreturns.h"
%}
%import "isensorreturns.h"

%include "hitpoints.h"
%{
#include "hitpoints.h"
%}

%include "rectangle.h"
%{
#include "rectangle.h"
%}
namespace sakusen {
  %extend Rectangle {
    %template(contains) contains<sint32>;
  }
  %template(SRectangle32) Rectangle<sint32>;
}

%include "box.h"
%{
#include "box.h"
%}
%template(SBox32) sakusen::Box<sint32>;

%include "ray.h"
%{
#include "ray.h"
%}

%include "quadratic.h"
%{
#include "quadratic.h"
%}

/* in lieu of including playerid.h */
typedef uint8 PlayerID;

%include "regiontype.h"
%include "region.h"
%include "sphereregion.h"
%include "rectangleregion.h"
%{
#include "region.h"
#include "region-methods.h"
#include "sphereregion.h"
#include "rectangleregion.h"
%}
%template(SRegion32) sakusen::Region<sint32>;
%template(SSphereRegion32) sakusen::SphereRegion<sint32>;
%template(SRectangleRegion32) sakusen::RectangleRegion<sint32>;

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


/* dependencies of libsakusen-comms */
%include "buffer.h"
%{
#include "buffer.h"
%}
%include "idbase.h"
%{
#include "idbase.h"
%}
namespace sakusen{
class ClientId;
%template(ClientIdBase) IdBase<uint8, ClientId >;
class PlayerId;
%template(PlayerIdBase) IdBase<uint8, PlayerId >;
class UnitId;
%template(UnitIdBase) IdBase<uint32, UnitId >;
class SensorReturnsId;
%template(SensorReturnsIdBase) IdBase<uint32, SensorReturnsId >;
class ClientBallisticId;
%template(ClientBallisticIdBase) IdBase<uint32, ClientBallisticId >;
}
%include "clientid.h"
%{
#include "clientid.h"
%}

%include "playerid.h"
%{
#include "playerid.h"
%}
%include "image.h"
%{
#include "image.h"
%}
%include "writer.h"
%{
#include "writer.h"
%}
%include "resourceinterface.h"
%{
#include "resourceinterface.h"
%}
%include "deserializationcontext.h"
%{
#include "deserializationcontext.h"
%}
%include "iheightfield.h"
%{
#include "iheightfield.h"
%}
%include "heightfield.h"
%{
#include "heightfield.h"
%}
%include "updatetype.h"
%{
#include "updatetype.h"
%}
%include "unitid.h"
%{
#include "unitid.h"
%}
%include "changeownerreason.h"
%{
#include "changeownerreason.h"
%}
%include "iunittypedata.h"
%{
#include "iunittypedata.h"
%}
%include "unittypedata.h"
%{
#include "unittypedata.h"
%}
%include "weapontype.h"
%{
#include "weapontype.h"
%}
%include "unittype.h"
%{
#include "unittype.h"
%}
%include "orientation.h"
%{
#include "orientation.h"
%}
%include "weapontargettype.h"
%{
#include "weapontargettype.h"
%}
%include "weaponstatus.h"
%{
#include "weaponstatus.h"
%}
%include "iunitstatus.h"
%{
#include "iunitstatus.h"
%}
%include "unitstatus.h"
%{
#include "unitstatus.h"
%}
%include "icompleteunit.h"
%{
#include "icompleteunit.h"
%}
%include "completeunit.h"
%{
#include "completeunit.h"
%}
%include "sensorreturnsid.h"
%{
#include "sensorreturnsid.h"
%}
%include "sensorreturns.h"
%{
#include "sensorreturns.h"
%}
%include "clientballisticid.h"
%{
#include "clientballisticid.h"
%}
%include "updatedata.h"
%{
#include "updatedata.h"
%}
%include "update.h"
%{
#include "update.h"
%}
%include "ordermessage.h"
%{
#include "ordermessage.h"
%}


