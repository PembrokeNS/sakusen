%module(directors="1") sakusenclient
%naturalvar;
%feature("autodoc","1");
%feature("director");
%{
#include <string>
%}
%import "libsakusen.i"
%include "libsakusen-client-global.h"
%{
#include "libsakusen-client-global.h"
%}
%include "updatedunit.h"
%{
#include "updatedunit.h"
%}
%include "unitfactory.h"
%{
#include "unitfactory.h"
%}
%include "partialworld.h"
%{
#include "partialworld.h"
%}
