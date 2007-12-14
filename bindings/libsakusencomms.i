%module sakusencomms
%naturalvar;
%import "libsakusen.i"
#define LIBSAKUSEN_API

/* libsakusen-comms */
%include "libsakusen-comms-global.h"
%{
#include "libsakusen-comms-global.h"
%}

%include "messagetype.h"
%{
#include "messagetype.h"
%}

%include "messagedata.h"
%{
#include "messagedata.h"
%}

%include "message.h"
%{
#include "message.h"
%}

%include "timeutils.h"
%{
#include "timeutils.h"
%}

%include "socket.h"
%{
#include "socket.h"
%}
