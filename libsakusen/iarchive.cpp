#include "iarchive.h"

#include "stringutils.h"
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

IArchive::IArchive(const uint8* b, size_t l) :
  originalBuffer(new uint8[l]),
  buffer(originalBuffer),
  remainingLength(l)
{
  memcpy(originalBuffer, b, l*sizeof(uint8));
}

IArchive::~IArchive()
{
  delete[] originalBuffer;
}

IArchive& IArchive::operator>>(uint16& i)
{
  assertLength(sizeof(uint16));
  i = ntohs(*reinterpret_cast<const uint16*>(buffer));
  advance(sizeof(uint16));
  return *this;
}

IArchive& IArchive::operator>>(sint16& i)
{
  assertLength(sizeof(sint16));
  i = ntohs(*reinterpret_cast<const sint16*>(buffer));
  advance(sizeof(sint16));
  return *this;
}

IArchive& IArchive::operator>>(uint32& i)
{
  assertLength(sizeof(uint32));
  i = ntohl(*reinterpret_cast<const uint32*>(buffer));
  advance(sizeof(uint32));
  return *this;
}

IArchive& IArchive::operator>>(sint32& i)
{
  assertLength(sizeof(sint32));
  i = ntohl(*reinterpret_cast<const sint32*>(buffer));
  advance(sizeof(sint32));
  return *this;
}

IArchive& IArchive::operator>>(double& d)
{
  /* FIXME: There's no platform-independent way to
   * encode a double in binary, so we
   * have to settle for a string encoding.  This is, of course, a recipe for
   * disaster. */
  String s;
  *this >> s;
  d = numFromString<double>(s);
  return *this;
}

IArchive& IArchive::operator>>(String& s)
{
  uint8 length;
  *this >> length;
  assertLength(length);
  s = String(
      reinterpret_cast<const char*>(buffer),
      static_cast<size_t>(length)
    );
  advance(length);
  return *this;
}

