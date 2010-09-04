%module(directors="1") sakusen
%naturalvar;
%feature("autodoc","1");
%{
#include <string>
%}
%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"

/* We can't just define _MSC_VER, because that picks up a 
 * whole load of template stuff that blows SWIG's tiny mind.*/
#ifdef _WIN32
  %include <windows.i>
#else
  %include "stdint.i"
#endif

%include "std_except.i"
#if !defined(SWIGCSHARP) && !defined(SWIGTCL)
  /* SWIG doesn't support this in C# or TCL yet */
  %include "std_list.i"
#endif
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
    template<class Y> explicit shared_ptr(Y * p);
    T* get() const;
    T* operator-> () const;
};

template<
    typename T0,
    typename T1 = boost::tuples::null_type,
    typename T2 = boost::tuples::null_type,
    typename T3 = boost::tuples::null_type,
    typename T4 = boost::tuples::null_type,
    typename T5 = boost::tuples::null_type
  >
class tuple {
  public:
    T0 get_head();
    tuple<T1, T2, T3, T4, T5> get_tail();
};

template<typename T0>
class tuple<
    T0,
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
#endif

%rename(numify) *::operator uint32;


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

%ignore debugStream;
%ignore errorStream;
%include <sakusen/libsakusen-global.h>
%{
#include <sakusen/libsakusen-global.h>
%}
#if !defined(SWIGCSHARP) && !defined(SWIGTCL)
  %template(stringList) std::list<sakusen::String>;
#endif
%include <sakusen/numbertarget.h>
%{
#include <sakusen/numbertarget.h>
%}
%include <sakusen/arithmetictraits.h>
%array_class(uint8,uint8);
%array_class(sint16,sint16);
/* it looks like it really has to be called uint8, otherwise SWIG gets
 * confused. Crazy, huh? */
%include <sakusen/revision.h>
%include <sakusen/gameobject.h>
%include <sakusen/topology.h>
%{
#include <sakusen/topology.h>
%}

%include <sakusen/numbertarget.h>
%{
#include <sakusen/numbertarget.h>
%}

%include <sakusen/point.h>
%{
#include <sakusen/point.h>
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
  %template("operator<") operator< <T>;
  %template("operator>") operator><T>;
}
%template(SPoint16) Point<sint16>;
%template(UPoint16) Point<uint16>;
%template(SPoint32) Point<sint32>;
%template(UPoint32) Point<uint32>;
%template(SPoint64) Point<sint64>;
}
%include <sakusen/angle.h>
%{
#include <sakusen/angle.h>
%}
%include <sakusen/buffer.h>
%{
#include <sakusen/buffer.h>
%}
%include <sakusen/iarchive.h>
%{
#include <sakusen/iarchive.h>
%}
%include <sakusen/oarchive.h>
%{
#include <sakusen/oarchive.h>
%}
%import <sakusen/ref.h>
%include <sakusen/box.h>
%{
#include <sakusen/box.h>
%}
%include <sakusen/bounded.h>
%{
#include <sakusen/bounded.h>
%}
%include <sakusen/idbase.h>
%{
#include <sakusen/idbase.h>
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
%include <sakusen/sensorreturnsid.h>
%{
#include <sakusen/sensorreturnsid.h>
%}
%include <sakusen/perception.h>
%{
#include <sakusen/perception.h>
%}
%include <sakusen/regiontype.h>
%include <sakusen/region.h>
%include <sakusen/sphereregion.h>
%include <sakusen/rectangleregion.h>
%{
#include <sakusen/region.h>
#include <sakusen/region-methods.h>
#include <sakusen/sphereregion.h>
#include <sakusen/rectangleregion.h>
%}

%include <sakusen/rectangle.h>
%{
#include <sakusen/rectangle.h>
%}
namespace sakusen {
  %extend Rectangle {
    %template(contains) contains<sint32>;
  }
  %template(SRectangle32) Rectangle<sint32>;
  %ignore Rectangle<sint16>::fastIntersects;
  %template(SRectangle16) Rectangle<sint16>;
}

%extend sakusen::Region {
  static boost::shared_ptr<sakusen::Region<T> >
  createRegionPtr(sakusen::Region<T> * r ){
    return boost::shared_ptr<sakusen::Region<T> >(r);
  }
}

%template(SRegion16) sakusen::Region<sint16>;
%template(SSphereRegion16) sakusen::SphereRegion<sint16>;
%template(SRectangleRegion16) sakusen::RectangleRegion<sint16>;

%include <sakusen/hitpoints.h>
%{
#include <sakusen/hitpoints.h>
%}
%include <sakusen/sensorreturnmap.h>
%{
#include <sakusen/sensorreturnmap.h>
%}
%include <sakusen/isensorreturns.h>
%{
#include <sakusen/isensorreturns.h>
%}
%include <sakusen/image.h>
%{
#include <sakusen/image.h>
%}
%include <sakusen/writer.h>
%{
#include <sakusen/writer.h>
%}
%include <sakusen/resourcesearchresult.h>
%{
#include <sakusen/resourcesearchresult.h>
%}
%include <sakusen/resourceinterface.h>
%{
#include <sakusen/resourceinterface.h>
#include <sakusen/resourceinterface-methods.h>
%}
%extend sakusen::ResourceInterface{
  %template(usave) save<sakusen::Universe>;
}
%include <sakusen/deserializationcontext.h>
%{
#include <sakusen/deserializationcontext.h>
%}
%include <sakusen/sensorreturn.h>
%{
#include <sakusen/sensorreturn.h>
%}
%include <sakusen/eachsensorreturn.h>
%{
#include <sakusen/eachsensorreturn.h>
%}
%include <sakusen/visibility.h>
#{
#include <sakusen/visibility.h>
#}
%include <sakusen/iunittypedata.h>
%{
#include <sakusen/iunittypedata.h>
%}
%include <sakusen/unittypedata.h>
%{
#include <sakusen/unittypedata.h>
%}
%include <sakusen/icompleteunit.h>
%{
#include <sakusen/icompleteunit.h>
%}
namespace sakusen{
/*Unfortunately this breaks due to making a pointer to a reference */
%ignore Ref<ICompleteUnit>::operator*;
%template(ICompleteUnitRef) Ref<ICompleteUnit>;
}
%ignore sakusen::Ref<sakusen::ICompleteUnit const>::operator*;
%template(ICompleteUnitConstRef) sakusen::Ref<sakusen::ICompleteUnit const>;
%ignore sakusen::Ref<sakusen::ISensorReturns>::operator*;
%template(ISensorReturnsRef) sakusen::Ref<sakusen::ISensorReturns>;
%include <sakusen/sensorreturns.h>
%{
#include <sakusen/sensorreturns.h>
%}


%template(SBox32) sakusen::Box<sint32>;

%include <sakusen/ray.h>
%{
#include <sakusen/ray.h>
%}

%include <sakusen/quadratic.h>
%{
#include <sakusen/quadratic.h>
%}

%template(SRegion32) sakusen::Region<sint32>;
%template(SSphereRegion32) sakusen::SphereRegion<sint32>;
%template(SRectangleRegion32) sakusen::RectangleRegion<sint32>;

%include <sakusen/ordertype.h>
%{
#include <sakusen/ordertype.h>
%}


/* dependencies of libsakusen-comms */
%include <sakusen/clientid.h>
%{
#include <sakusen/clientid.h>
%}

%include <sakusen/playerid.h>
%{
#include <sakusen/playerid.h>
%}
%include <sakusen/orientation.h>
%{
#include <sakusen/orientation.h>
%}
%include <sakusen/frame.h>
%{
#include <sakusen/frame.h>
%}
%include <sakusen/mathsutils.h>
%{
#include <sakusen/mathsutils.h>
%}
%include <sakusen/iheightfield.h>
%{
#include <sakusen/iheightfield.h>
%}
%include <sakusen/heightfield.h>
%{
#include <sakusen/heightfield.h>
#include <sakusen/heightfield-methods.h>
%}
%include <sakusen/updatetype.h>
%{
#include <sakusen/updatetype.h>
%}
%include <sakusen/unitid.h>
%{
#include <sakusen/unitid.h>
%}
%include <sakusen/changeownerreason.h>
%{
#include <sakusen/changeownerreason.h>
%}
%include <sakusen/weapontype.h>
%{
#include <sakusen/weapontype.h>
%}

#if !defined(SWIGCSHARP) && !defined(SWIGTCL)
  /* These two lines needed because WeaponTypeId is actually a const pointer,
   * and swig has trouble with containers of such. There are unconfirmed
   * reports that this problem is fixed in SWIG SVN, so it may be worth trying
   * removing them once swig 1.3.37 is out. */
  %traits_swigtype(sakusen::WeaponType);
  %fragment(SWIG_Traits_frag(sakusen::WeaponType));
#endif
/* Different circumstances require different %ignores here to actually persuade
 * Swig to ignore the constructor.  Exactly what circumstances matter is
 * unclear. */
%ignore std::vector<sakusen::WeaponType>::vector(size_type);
%ignore std::vector<sakusen::WeaponType>::vector(unsigned int);
%ignore std::vector<sakusen::WeaponType>::resize(size_type);
%ignore std::vector<sakusen::UnitType>::vector(size_type);
%ignore std::vector<sakusen::UnitType>::vector(unsigned int);
%ignore std::vector<sakusen::UnitType>::resize(size_type);
#if !defined(SWIGCSHARP) && !defined(SWIGTCL)
  %template(stdVectorWeaponTypeId) std::vector<sakusen::WeaponTypeId>;
#endif
%template(stdVectorWeaponType) std::vector<sakusen::WeaponType>;

%include <sakusen/unittype.h>
%{
#include <sakusen/unittype.h>
%}
%template(stdVectorUnitType) std::vector<sakusen::UnitType>;
%include <sakusen/universe.h>
%{
#include <sakusen/universe.h>
%}
namespace sakusen{
%extend ResourceInterface {
  /* This should be a %template, but as of swig-1.3.31 that broke for reasons
   * unknown. */
  /* Returning a ConstPtr rather than a Ptr is very useful to me - Lmm */
    boost::tuple<sakusen::Universe::ConstPtr, ResourceSearchResult, String>
    searchUniverse(
        const String& name,
        sakusen::Universe::ConstPtr u = sakusen::Universe::ConstPtr()
      ) {
      return $self->search<sakusen::Universe>(name, u);
    }
  }
  %template(resourceInterfacePtr) ::boost::shared_ptr<ResourceInterface>;
  %template(UniverseConstPtr) ::boost::shared_ptr<const Universe>;
  %template(UniversePtr) ::boost::shared_ptr<Universe>;
  %template(SRegion32ConstPtr) ::boost::shared_ptr<const Region<sint32> >;
}
%template(SRegion32Ptr) boost::shared_ptr<sakusen::Region<sint32> >;
%template(SRegion16Ptr) boost::shared_ptr<sakusen::Region<sint16> >;
%extend sakusen::Universe {
  static ::boost::shared_ptr<const sakusen::Universe>
  createConstPtr(sakusen::Universe *u) {
    return ::boost::shared_ptr<const sakusen::Universe>(u);
  }
}
%include <sakusen/weapontargettype.h>
%{
#include <sakusen/weapontargettype.h>
%}
%include <sakusen/materielprovider.h>
%{
#include <sakusen/materielprovider.h>
%}
%include <sakusen/weaponstatus.h>
%{
#include <sakusen/weaponstatus.h>
%}
%include <sakusen/iunitstatus.h>
%{
#include <sakusen/iunitstatus.h>
%}
%include <sakusen/unitstatus.h>
%{
#include <sakusen/unitstatus.h>
#include <sakusen/unitstatus-methods.h>
%}
%include <sakusen/completeunit.h>
%{
#include <sakusen/completeunit.h>
%}
%include <sakusen/clientballisticid.h>
%{
#include <sakusen/clientballisticid.h>
%}
%include <sakusen/updatedata.h>
%{
#include <sakusen/updatedata.h>
%}
%include <sakusen/update.h>
%{
#include <sakusen/update.h>
%}
#if !defined(SWIGCSHARP) && !defined(SWIGTCL)
  /* SWIG doesn't support std::list in C# or TCL yet */
  namespace sakusen{
  %template(listUpdate) ::std::list<Update>;
  }
#endif
%include <sakusen/ordermessage.h>
%{
#include <sakusen/ordermessage.h>
%}
%include <sakusen/world.h>
%{
#include <sakusen/world.h>
%}
%include <sakusen/targettype.h>
%{
#include <sakusen/targettype.h>
%}
%include <sakusen/weaponorders.h>
%{
#include <sakusen/weaponorders.h>
%}
%include <sakusen/unitorders.h>
%{
#include <sakusen/unitorders.h>
%}
%include <sakusen/exceptions.h>
%{
#include <sakusen/exceptions.h>
%}
%include <sakusen/map.h>
%{
#include <sakusen/map.h>
%}
%include <sakusen/orderdata.h>
%{
#include <sakusen/orderdata.h>
%}
%include <sakusen/order.h>
%{
#include <sakusen/order.h>
%}
%include <sakusen/hitpoints.h>
%{
#include <sakusen/hitpoints.h>
%}
%include <sakusen/unittemplate.h>
%{
#include <sakusen/unittemplate.h>
%}
%ignore std::vector<sakusen::UnitTemplate>::vector(size_type);
%ignore std::vector<sakusen::UnitTemplate>::vector(unsigned int);
%ignore std::vector<sakusen::UnitTemplate>::resize(size_type);
%template(unitTemplateVector) std::vector<sakusen::UnitTemplate>;
%include <sakusen/playertemplate.h>
%{
#include <sakusen/playertemplate.h>
%}
%ignore std::vector<sakusen::PlayerTemplate>::resize(size_type);
%ignore std::vector<sakusen::PlayerTemplate>::vector(size_type);
%ignore std::vector<sakusen::PlayerTemplate>::vector(unsigned int);
%template(playerTemplateVector) std::vector<sakusen::PlayerTemplate>;
%include <sakusen/mapplaymode.h>
%{
#include <sakusen/mapplaymode.h>
%}
%ignore std::vector<sakusen::MapPlayMode>::resize(size_type);
%ignore std::vector<sakusen::MapPlayMode>::vector(size_type);
%ignore std::vector<sakusen::MapPlayMode>::vector(unsigned int);
%template(mapPlayModeVector) std::vector<sakusen::MapPlayMode>;
%include <sakusen/maptemplate.h>
%{
#include <sakusen/maptemplate.h>
%}
%extend sakusen::MapTemplate {
  static boost::shared_ptr<sakusen::MapTemplate>
  createPtr(sakusen::MapTemplate * r ){
    return boost::shared_ptr<sakusen::MapTemplate>(r);
  }
}
%extend sakusen::ResourceInterface{
  %template(mtsave) save<sakusen::MapTemplate>;
}
%template(MapTemplatePtr) boost::shared_ptr<sakusen::MapTemplate>;
