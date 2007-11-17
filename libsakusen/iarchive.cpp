#include "iarchive.h"

#include "oarchive.h"
#include "stringutils.h"

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

using namespace sakusen;

/** \brief Default constructor
 *
 * Creates an empty IArchive, from which nothing can be extracted */
IArchive::IArchive() :
  originalBuffer(),
  pos(NULL),
  remainingLength(0)
{
}

/** \brief Constructs IArchive from raw data buffer */
IArchive::IArchive(const uint8* b, size_t l) :
  originalBuffer(b, l),
  pos(originalBuffer.getConst()),
  remainingLength(l)
{
}

/** \brief Constructs IArchive from raw data in a shared_array */
IArchive::IArchive(const boost::shared_array<uint8>& b, size_t l) :
  originalBuffer(b, l),
  pos(originalBuffer.getConst()),
  remainingLength(l)
{
}

/** \brief Constructs IArchive from raw data in a shared_array */
IArchive::IArchive(const boost::shared_array<const uint8>& b, size_t l) :
  originalBuffer(b, l),
  pos(originalBuffer.getConst()),
  remainingLength(l)
{
}

IArchive::IArchive(const Buffer& b) :
  originalBuffer(b),
  pos(originalBuffer.getConst()),
  remainingLength(originalBuffer.getSize())
{
}

/** \brief Dump the buffer as hex to stdout
 *
 * \note For debugging purposes.  In particular, it can be called from a
 * debugger. */
void IArchive::dumpBuffer() const
{
  Debug(stringUtils_bufferToHex(pos, remainingLength));
}

IArchive& IArchive::operator>>(uint16& i)
{
  assertLength(sizeof(uint16));
  i = ntohs(*reinterpret_cast<const uint16*>(pos));
  advance(sizeof(uint16));
  return *this;
}

IArchive& IArchive::operator>>(sint16& i)
{
  assertLength(sizeof(sint16));
  i = ntohs(*reinterpret_cast<const sint16*>(pos));
  advance(sizeof(sint16));
  return *this;
}

IArchive& IArchive::operator>>(uint32& i)
{
  assertLength(sizeof(uint32));
  i = ntohl(*reinterpret_cast<const uint32*>(pos));
  advance(sizeof(uint32));
  return *this;
}

IArchive& IArchive::operator>>(sint32& i)
{
  assertLength(sizeof(sint32));
  i = ntohl(*reinterpret_cast<const sint32*>(pos));
  advance(sizeof(sint32));
  return *this;
}

IArchive& IArchive::operator>>(double& d)
{
  /** \bug There's no platform-independent way to
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
  uint32 length;
  *this >> length;
  assertLength(length);
  s = String(
      reinterpret_cast<const char*>(pos),
      static_cast<size_t>(length)
    );
  advance(length);
  /** \todo This would be a good time to check that the string is valid UTF-8
   * (perhaps as a compile-time option) */
  return *this;
}

IArchive& IArchive::operator>>(Buffer& b)
{
  uint32 length;
  *this >> length;
  assertLength(length);
  b.resize(length);
  memcpy(b.get(), pos, length);
  advance(length);
  return *this;
}
