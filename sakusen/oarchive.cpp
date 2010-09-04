#include "oarchive.h"

#include "iarchive.h"
#include "stringutils.h"

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

using namespace sakusen;

/** \brief Default constructor.
 *
 * Constructs an empty OArchive, ready to have things inserted into it.
 */
OArchive::OArchive() :
  buffer()
{
}

/** \brief Constructs with given start capacity.
 *
 * Allows the initial capacity of the OArchive's internal buffer to be
 * specified.  This might save a few reallocations in some circumstances if you
 * can know roughly how big the OArchive will need to be.  Don't bother to use
 * this without good reason, though.
 */
OArchive::OArchive(size_t startCapacity) :
  buffer(size_t(0), startCapacity)
{
}

/** \brief Dump the buffer as hex to stdout
 *
 * \note For debugging purposes.  In particular, it can be called from a
 * debugger. */
void OArchive::dumpBuffer() const
{
  SAKUSEN_DEBUG(stringUtils_bufferToHex(buffer.getConst(), buffer.getSize()));
}

OArchive& OArchive::operator<<(const uint16& i)
{
  buffer.appendRaw<uint16>(ntohs(i));
  return *this;
}

OArchive& OArchive::operator<<(const sint16& i)
{
  return *this << uint16(i);
}

OArchive& OArchive::operator<<(const uint32& i)
{
  buffer.appendRaw<uint32>(ntohl(i));
  return *this;
}

OArchive& OArchive::operator<<(const sint32& i)
{
 return *this << uint32(i);
}

OArchive& OArchive::operator<<(const uint64& i)
{
  return *this << uint32(i >> 32) << uint32(i);
}

OArchive& OArchive::operator<<(const sint64& i)
{
  return *this << uint64(i);
}

OArchive& OArchive::operator<<(const double& d)
{
  return *this << numToString(d);
}

OArchive& OArchive::operator<<(const String& s)
{
  size_t sLength = s.length();
  /** \todo Probably this should be an exception rather than a SAKUSEN_FATAL, but maybe
   * we should handle it altogether differently?
   */
  *this << static_cast<uint32>(sLength);
  buffer.appendBytes(s.c_str(), sLength);
  return *this;
}

OArchive& OArchive::operator<<(const Buffer& b)
{
  size_t sLength = b.getSize();
  *this << static_cast<uint32>(sLength);
  buffer.appendBytes(b.getConst(), b.getSize());
  return *this;
}

