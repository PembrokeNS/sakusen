%module sakusen
%naturalvar;
%feature("autodoc","1");
%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%include "stdint.i"
%include "std_except.i"
%include "carrays.i"

namespace std {
%template(pairDoubleDouble) pair<double, double>;
}

/* Declare struct hash to quiet SWIG's warning messages */
namespace __gnu_cxx {
template<typename T> struct hash;
}

namespace boost {
template<class T> class shared_ptr{
  public:
    shared_ptr(shared_ptr<T> const &);
    T* get() const;
    T* operator-> () const;
};

template<
    typename T0,
    typename T1 = boost::tuples::null_type,
    typename T2 = boost::tuples::null_type,
    typename T3 = boost::tuples::null_type,
    typename T4 = boost::tuples::null_type,
    typename T5 = boost::tuples::null_type,
    typename T6 = boost::tuples::null_type
  >
class tuple {
  public:
    T0 get_head();
    tuple<T1, T2, T3, T4, T5, T6> get_tail();
};

template<typename T0>
class tuple<
    T0,
    boost::tuples::null_type,
    boost::tuples::null_type,
    boost::tuples::null_type,
    boost::tuples::null_type,
    boost::tuples::null_type,
    boost::tuples::null_type
  >
{
  public:
    T0 get_head();
    boost::tuples::null_type get_tail();
};

%template(tupleDouble) tuple<double>;
%template(tupleDoubleDouble) tuple<double, double>;

}

#ifdef SWIGPERL
#define BINDING_NOOVERLOAD
%perlcode %{
$VERSION = 0.01;
%}

%rename(numify) *::operator uint32;
#endif


/*Make derived classes in different packages from their base classes work*/
%typemap(csbody) SWIGTYPE %{
  private System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  /* This was internal but is now public because it is called from
   * libsakusen-comms bindings */
  public $csclassname(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  public static System.Runtime.InteropServices.HandleRef getCPtr($csclassname obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }
%}

/* The analagous typemap for derived classes */
%typemap(csbody_derived) SWIGTYPE %{
  private HandleRef swigCPtr;

  public $csclassname(IntPtr cPtr, bool cMemoryOwn) :
      base($modulePINVOKE.$csclassnameUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  public static HandleRef getCPtr($csclassname obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }
%}
#ifdef SWIGCSHARP

/* Renames for wrapped operators.  Although C# does support operator
 * overloading, it doesn't work in the same way as C++'s, so for now we don't
 * try to make use of it */
%rename(Numify) *::operator unsigned char;
%rename(Numify) *::operator unsigned int;
%rename(Numify) *::operator uint32;
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
%ignore *::operator!;
%ignore *::operator>>;
%ignore *::operator<<;
%ignore operator++;
%ignore operator==;
%ignore operator!=;
%ignore operator>>;
%ignore operator<<;
%ignore operator|;
%ignore operator|=;

/* We have to rename Buffer because the name clashes with System.Buffer */
%rename(SakusenBuffer) Buffer;

#endif

/* General ignores to resolve ambiguities caused by SWIG's collapsing of types
 * */

%ignore sakusen::Point::operator[](int const) const;
%ignore sakusen::UnitStatus::getWeaponsStatus(void) const;

#define LIBSAKUSEN_API

%ignore debugStream;
%ignore errorStream;
%include "libsakusen-global.h"
%{
#include "libsakusen-global.h"
%}
%include "intmunger.h"
%array_class(uint8,uint8);
/* it looks like it really has to be called uint8, otherwise SWIG gets confused. Crazy, huh? */
/* tiny files */
%include "angle.h"
/* Causes make clean to fail, though this is probably due to problems in makefiles %include "revision.h"*/
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
%import "boost/shared_array.hpp"
%template(sharedarrayuint8) boost::shared_array<uint8>;
%include "buffer.h"
%{
#include "buffer.h"
%}
%include "iarchive.h"
%{
#include "iarchive.h"
%}
%include "oarchive.h"
%{
#include "oarchive.h"
%}
%import "ref.h"
%include "box.h"
%{
#include "box.h"
%}
%include "bounded.h"
%{
#include "bounded.h"
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
%include "sensorreturnsid.h"
%{
#include "sensorreturnsid.h"
%}
%include "perception.h"
%{
#include "perception.h"
%}
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

%include "hitpoints.h"
%{
#include "hitpoints.h"
%}
%include "isensorreturns.h"
%{
#include "isensorreturns.h"
%}
%include "image.h"
%{
#include "image.h"
%}
%include "writer.h"
%{
#include "writer.h"
%}
%include "resourcesearchresult.h"
%{
#include "resourcesearchresult.h"
%}
%include "resourceinterface.h"
%{
#include "resourceinterface.h"
#include "resourceinterface-methods.h"
%}
%include "deserializationcontext.h"
%{
#include "deserializationcontext.h"
%}
%include "sensorreturns.h"
%{
#include "sensorreturns.h"
%}
%include "sensorreturn.h"
%{
#include "sensorreturn.h"
%}
%include "eachsensorreturn.h"
%{
#include "eachsensorreturn.h"
%}
%include "visibility.h"
#{
#include "visibility.h"
#}

%include "iunittypedata.h"
%{
#include "iunittypedata.h"
%}
%include "unittypedata.h"
%{
#include "unittypedata.h"
%}
%include "icompleteunit.h"
%{
#include "icompleteunit.h"
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

%template(SRegion32) sakusen::Region<sint32>;
%template(SSphereRegion32) sakusen::SphereRegion<sint32>;
%template(SRectangleRegion32) sakusen::RectangleRegion<sint32>;

/* we shouldn't need to call any of the functions herein */
%import "playertemplate.h"
%import "ordertype.h"


/* dependencies of libsakusen-comms */
%include "clientid.h"
%{
#include "clientid.h"
%}

%include "playerid.h"
%{
#include "playerid.h"
%}
%include "mathsutils.h"
%{
#include "mathsutils.h"
%}
%include "iheightfield.h"
%{
#include "iheightfield.h"
%}
%include "heightfield.h"
%{
#include "heightfield.h"
#include "heightfield-methods.h"
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
%include "weapontype.h"
%{
#include "weapontype.h"
%}
%include "unittype.h"
%{
#include "unittype.h"
%}
%include "universe.h"
%{
#include "universe.h"
%}
namespace sakusen{
%extend ResourceInterface {
/*This should be a %template, but as of swig-1.3.31 that broke for reasons unknown*/
    boost::tuple<sakusen::Universe::Ptr, ResourceSearchResult, String>
    searchUniverse(
        const String& name,
        sakusen::Universe::ConstPtr u = sakusen::Universe::ConstPtr()
      ) {
      return $self->search<sakusen::Universe>(name, u);
    }
  }
  %template(resourceInterfacePtr) ::boost::shared_ptr<ResourceInterface>;
}
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
#include "unitstatus-methods.h"
%}
%include "completeunit.h"
%{
#include "completeunit.h"
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
%include "world.h"
%{
#include "world.h"
%}


