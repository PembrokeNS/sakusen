%module sakusen_comms
%naturalvar;
%{
#include <string>
%}
%feature("autodoc","1");
%include "cmalloc.i"
%include "cdata.i"
#ifndef SWIGCSHARP
#ifndef SWIGTCL
/*support for std::list<sakusen::Update*/
%include "std_list.i"
%{
#include "update.h"
%}
#endif
#endif

%import "libsakusen.i"
%malloc(void);

#ifdef SWIGCSHARP
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

%ignore operator+(timeval tv, sint32 usec);
%ignore operator+=(timeval& tv, sint32 usec);
%ignore operator+=(timeval& tv, const timeval& uv);
%ignore operator-(const timeval& tv, const timeval& uv);
%ignore operator>(const timeval& tv, const timeval& uv);
%ignore operator<(const timeval& tv, const timeval& uv);

/* We have to rename Buffer because the name clashes with System.Buffer */
%rename(SakusenBuffer) Buffer;
#endif

%typemap (csimports) SWIGTYPE "
using System;
using System.Runtime.InteropServices;
using Sakusen;
"

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
#ifdef SWIGPYTHON
%include "std_set.i"
namespace saksen{
namespace comms{
%template(stringSet) ::std::set<::sakusen::String>;
}
}
#endif
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

%include "socketexn.h"
%{
#include "socketexn.h"
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
#ifndef _WIN32
	struct timeval {
		 time_t      tv_sec;     /* seconds */
		 suseconds_t tv_usec;    /* microseconds */
		 };
#else
	struct timeval {
			long    tv_sec;         /* seconds */
			long    tv_usec;        /* and microseconds */
	};
#endif

%extend timeval {
        timeval(long s,long m){
                timeval *t;
                t=new timeval;
                t->tv_sec=s;
                t->tv_usec=m;
                return t;
                }
        }

