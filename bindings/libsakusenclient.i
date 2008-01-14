%module(directors="1") sakusenclient
%naturalvar;
%feature("autodoc","1");
%feature("director") sakusen::client::UnitFactory;
%feature("director") sakusen::client::UpdatedUnit;
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
#include "updatedunit.h"
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
%include "unitfactory.h"
%{
#include "unitfactory.h"
%}
%include "partialworld.h"
%{
#include "partialworld.h"
%}
