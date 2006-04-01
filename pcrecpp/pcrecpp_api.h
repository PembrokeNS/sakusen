#ifndef PCRECPP_API_H
#define PCRECPP_API_H

#ifdef PCRECPP_EXPORTS
#define PCRECPP_API _declspec(dllexport)
#else
#define PCRECPP_API _declspec(dllimport)
#endif

#endif // PCRECPP_API_H
