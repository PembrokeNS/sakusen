%module sakusencomms
%naturalvar;
%feature("autodoc","1");
%include "cmalloc.i"
%include "cdata.i"
%import "libsakusen.i"
%malloc(void);
#define LIBSAKUSEN_API

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

/*namespace boost {
template<class T> class shared_ptr{
public:
T* get() const;
T* operator-> () const;
};
}*/
namespace sakusen{
namespace comms{
class Socket;
%template(Ptr) ::boost::shared_ptr<Socket>;
}
}
%include "socket.h"
%{
#include "socket.h"
%}
