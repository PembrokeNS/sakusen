#ifndef LANGINFO_H
#define LANGINFO_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef int nl_item;

#define CODESET ((nl_item) 1)
#ifdef _MSC_VER
  #ifdef LIBUNICODE_EXPORTS
    #define LIBUNICODE_API   __declspec(dllexport)
  #else
    #define LIBUNICODE_API   __declspec(dllimport)
  #endif
#else 
#define LIBUNICODE_API 
#endif

LIBUNICODE_API char *nl_langinfo(nl_item);

#ifdef __cplusplus
}
#endif

#endif /* LANGINFO_H */
