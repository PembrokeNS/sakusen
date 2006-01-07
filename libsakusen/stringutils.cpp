#include "stringutils.h"

#include <iomanip>

/* See below for why this is here */
#ifndef WIN32
#include <mhash.h>
#endif

using namespace std;

list<String> stringUtils_split(
    const String& splitee,
    const String& spliton
  )
{
  /* TODO: Replace with or add a regex-using version */
  String::size_type chunk_start;
  String::size_type chunk_end=0;
  list<String> result;
  
  while (chunk_end != splitee.npos) {
    chunk_start = splitee.find_first_not_of(spliton, chunk_end);
    if (chunk_start == splitee.npos) {
      break;
    }
    chunk_end = splitee.find_first_of(spliton, chunk_start);
    result.push_back(splitee.substr(chunk_start, chunk_end-chunk_start));
  }
  return result;
}

String stringUtils_getSecureHashAsString(const uint8* buffer, size_t length)
{
    /*mhash is only for linux and CYGWIN. It would be nice if *
    we could switch to some other library for crypto things, or if 
    someone could build an mhash dll for me using CYGWIn, or something.*/
  /* TODO: Maybe support other hashes */
#ifndef WIN32
  MHASH thread = mhash_init(MHASH_SHA256);
  /* FIXME: Commented out because it generates an old style cast warning (silly
   * reason...)
  if (thread == MHASH_FAILED) {
    Fatal("error initing hash");
  }
  */
  
  mhash(thread, buffer, length);

  uint8 hash[32];
  
  mhash_deinit(thread, hash);
  
  ostringstream stream;
  for (int i=0; i<32; i++) {
    /* Following command doesn't work becase I can't figure out how to pad with
     * zeroes
    stream << hex << setw(2) << uint32(hash[i]); */
    char byte[3];
    snprintf(byte, 3, "%02x", hash[i]);
    byte[2] = '\0';
    stream << byte;
  }
  return stream.str();
#else
    return "HashNotSupported";
#endif
}

