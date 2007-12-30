%module sakusencomms
%naturalvar;
%feature("autodoc","1");
%include "cmalloc.i"
%include "cdata.i"
%import "libsakusen.i"
%malloc(void);
#define LIBSAKUSEN_API

#ifdef SWIGCSHARP
/*This was broken in C# - no idea why. John? They're all the same error - I suspect pinvoke lulz */
%ignore sakusen::comms::MessageData::getBuffer;
%ignore sakusen::comms::AcceptMessageData::getId;
%ignore sakusen::comms::ExtensionMessageData::getData;
%ignore sakusen::comms::GameStartMessageData::getPlayerId;
%ignore sakusen::comms::GameStartMessageData::getTopRight;
%ignore sakusen::comms::GameStartMessageData::getBottomLeft;
%ignore sakusen::comms::GameStartMessageData::getHeightfield;
%ignore sakusen::comms::Message::getBuffer;
%ignore sakusen::comms::OrderMessageData::getOrderMessage;
#endif

/* libsakusen-comms */
%include "libsakusen-comms-global.h"
%{
#include "libsakusen-comms-global.h"
%}

%ignore sakusen::comms::operator<<(std::ostream& output, MessageType type);
%include "messagetype.h"
%{
#include "messagetype.h"
%}

namespace sakusen{
namespace comms{
class MessageData;
%template(mPtr) ::boost::shared_ptr<MessageData>;
%template(mConstPtr) ::boost::shared_ptr<const MessageData>;
}
}
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

namespace sakusen{
namespace comms{
class Socket;
%template(sPtr) ::boost::shared_ptr<Socket>;
}
}
%include "socket.h"
%{
#include "socket.h"
%}
%include "unixdatagramsocket.h"
%{
#include "unixdatagramsocket.h"
%}
%include "unixdatagramlisteningsocket.h"
%{
#include "unixdatagramlisteningsocket.h"
%}
%{
#include "time.h"
%}
struct timeval {
     time_t      tv_sec;     /* seconds */
     suseconds_t tv_usec;    /* microseconds */
     };
%extend timeval {
        timeval(long s,long m){
                timeval *t;
                t=(timeval *)malloc(sizeof(timeval));
                t->tv_sec=s;
                t->tv_usec=m;
                return t;
                }
        }

