%module(directors="1") sakusen_client
%naturalvar;
%feature("autodoc","1");
%feature("director") sakusen::client::UnitFactory;
%feature("director") sakusen::client::UpdatedUnit;
%feature("director") sakusen::client::SensorReturnsFactory;
%feature("director") sakusen::client::UpdatedSensorReturns;
/* My current thinking is that this is a SWIG bug - it doesn't generate director wrappings properly for functions which are implemented in classes in libsakusen, leading to an infinite loop whenever they are called. TODO: create test case, report */
%feature("nodirector") sakusen::client::UpdatedSensorReturns::getPerception;
%feature("nodirector") sakusen::client::UpdatedSensorReturns::getUnit;
%feature("nodirector") sakusen::client::UpdatedSensorReturns::getId;
%feature("nodirector") sakusen::client::UpdatedSensorReturns::getBoundingRectangle;
%feature("nodirector") sakusen::client::UpdatedSensorReturns::getRegion;
%feature("nodirector") sakusen::client::UpdatedUnit::getId;
%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}
#ifdef SWIGPYTHON
%exception {
    try { $action }
    catch (Swig::DirectorException &e) { SWIG_fail; }
}
#endif
%{
#include <string>
%}
%{
#include "boost/shared_ptr.hpp"
%}
%{
/* Needed because libsakusen.i includes a weapontypes traits fragment */
namespace sakusen{
class WeaponType;
/* And some Region template instantiations */
template<typename T> class SphereRegion;
}
#include "sakusen/rectangleregion.h"
%}
%import "libsakusen.i"
%{
/* seems these have to be early, no idea why */
#include "sakusen/client/updatedunit.h"
#include "sakusen/client/updatedsensorreturns.h"
%}
%include "sakusen/client/global.h"
%{
#include "sakusen/client/global.h"
%}
%include "sakusen/client/updatedunit.h"
namespace sakusen{
namespace client{
%template(UpdatedUnitPtr) ::boost::shared_ptr<UpdatedUnit>;
%extend UpdatedUnit {
/* I can't get making from a * to work in general, so I won't try */
static boost::shared_ptr<sakusen::client::UpdatedUnit> CreateUpdatedUnitPtr(sakusen::client::UpdatedUnit *u){
	return boost::shared_ptr<sakusen::client::UpdatedUnit>(u);
}
}
}
}
/* temporary workaround 
#define LIBSAKUSEN_CLIENT_API*/
%include "sakusen/client/updatedsensorreturns.h"
namespace sakusen{
namespace client{
class UpdatedSensorReturns; //ARRGH
%template(UpdatedSensorReturnsPtr) ::boost::shared_ptr<UpdatedSensorReturns>;
%extend UpdatedSensorReturns {
/* this shouldn't be necessary at all, but I can't get things working without it. Perhaps the shared_ptr constructor doesn't actually get wrapped in python? */
static boost::shared_ptr<sakusen::client::UpdatedSensorReturns> CreateUpdatedSensorReturnsPtr(sakusen::client::UpdatedSensorReturns *r){
        return boost::shared_ptr<sakusen::client::UpdatedSensorReturns>(r);
}
}
}
}
%include "sakusen/client/unitfactory.h"
%{
#include "sakusen/client/unitfactory.h"
%}
%include "sakusen/client/sensorreturnsfactory.h"
%{
#include "sakusen/client/sensorreturnsfactory.h"
%}
%include "sakusen/client/partialheightfield.h"
%{
#include "sakusen/client/partialheightfield.h"
%}
%include "sakusen/client/partialmap.h"
%{
#include "sakusen/client/partialmap.h"
%}
%include "sakusen/client/partialworld.h"
%{
#include "sakusen/client/partialworld.h"
%}
