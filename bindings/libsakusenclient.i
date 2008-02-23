%module(directors="1") sakusenclient
%naturalvar;
%feature("autodoc","1");
%feature("director") sakusen::client::UnitFactory;
%feature("director") sakusen::client::UpdatedUnit;
%feature("director") sakusen::client::SensorReturnsFactory;
/*If this has a director then when one tries to call it one gets an infinite loop, no idea why - maybe because it's inline?*/
%feature("nodirector") sakusen::client::UpdatedUnit::getId;
%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}
%exception {
    try { $action }
    catch (Swig::DirectorException &e) { SWIG_fail; }
}
%{
#include <string>
%}
%{
#include "boost/shared_ptr.hpp"
%}
%{
/* seems these have to be early, no idea why */
#include "updatedunit.h"
#include "updatedsensorreturns.h"
%}
%import "libsakusen.i"
%include "libsakusen-client-global.h"
%{
#include "libsakusen-client-global.h"
%}
%include "updatedunit.h"
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
%include "updatedsensorreturns.h"
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
%include "unitfactory.h"
%{
#include "unitfactory.h"
%}
%include "sensorreturnsfactory.h"
%{
#include "sensorreturnsfactory.h"
%}
%include "partialworld.h"
%{
#include "partialworld.h"
%}
