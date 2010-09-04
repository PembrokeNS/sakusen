#ifndef WINDOWS_AS_BERKELEY
#define WINDOWS_AS_BERKELEY

#ifdef _MSC_VER

//Redefines the windows socket error numbers as standard BSD error numbers. 
//Actually in winsock.h, but commented out in a rather bizarre manner.

//Undefine them first, as they are also defined somewhere in boost.
#undef EWOULDBLOCK             
#undef EINPROGRESS             
#undef EALREADY                
#undef ENOTSOCK                
#undef EDESTADDRREQ            
#undef EMSGSIZE                
#undef EPROTOTYPE              
#undef ENOPROTOOPT             
#undef EPROTONOSUPPORT         
#undef ESOCKTNOSUPPORT         
#undef EOPNOTSUPP              
#undef EPFNOSUPPORT            
#undef EAFNOSUPPORT            
#undef EADDRINUSE              
#undef EADDRNOTAVAIL           
#undef ENETDOWN                
#undef ENETUNREACH             
#undef ENETRESET               
#undef ECONNABORTED            
#undef ECONNRESET              
#undef ENOBUFS                 
#undef EISCONN                 
#undef ENOTCONN                
#undef ESHUTDOWN               
#undef ETOOMANYREFS            
#undef ETIMEDOUT               
#undef ECONNREFUSED            
#undef ELOOP                   
#undef EHOSTDOWN               
#undef EHOSTUNREACH            
#undef EPROCLIM                
#undef EUSERS                  
#undef EDQUOT                  
#undef ESTALE                  
#undef EREMOTE                 
#undef EBADMSG                 

#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE
#define EBADMSG                 11032 //Not defined by Windows. This error code should be unused.

#endif // _MSC_VER

#endif
