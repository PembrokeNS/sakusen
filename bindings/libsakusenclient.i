%module sakusenresources
%naturalvar;
%feature("autodoc","1");
%import "libsakusen.i"
%include "libsakusen-client-global.h"
%{
#include "libsakusen-client-global.h"
%}
%include "partialworld.h"
%{
#include "partialworld.h"
%}
