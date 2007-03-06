#include "oarchive.h"

#include "iarchive.h"
#include "stringutils.h"

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

#define DEFAULT_START_CAPACITY 1

using namespace sakusen;

OArchive::OArchive() :
  buffer(new uint8[DEFAULT_START_CAPACITY]),
  length(0),
  capacity(DEFAULT_START_CAPACITY)
{
}

OArchive::OArchive(size_t startCapacity) :
  buffer(new uint8[startCapacity]),
  length(0),
  capacity(startCapacity)
{
}

OArchive::OArchive(const OArchive& copy) :
  buffer(new uint8[copy.length]),
  length(copy.length),
  capacity(copy.length)
{
  memcpy(buffer, copy.buffer, length*sizeof(uint8));
}

OArchive::~OArchive()
{
  assert(buffer != NULL);
  delete[] buffer;
  buffer = NULL;
}

OArchive& OArchive::operator=(const OArchive& copy)
{
  uint8* newBuffer = new uint8[copy.length];
  capacity = length = copy.length;
  memcpy(newBuffer, copy.buffer, length*sizeof(uint8));
  assert(buffer != NULL);
  delete[] buffer;
  buffer = newBuffer;
  return *this;
}

OArchive& OArchive::operator=(const IArchive& copy)
{
  // We should only be doing this with an IArchive which we haven't extracted
  // anything from yet
  assert(copy.buffer == copy.originalBuffer);
  uint8* newBuffer = new uint8[copy.remainingLength];
  capacity = length = copy.remainingLength;
  memcpy(newBuffer, copy.originalBuffer, length*sizeof(uint8));
  assert(buffer != NULL);
  delete[] buffer;
  buffer = newBuffer;
  return *this;
}

void OArchive::expand()
{
  if (capacity == 0) {
    capacity = 1;
  } else {
    capacity *= 2;
  }
  uint8* newBuffer = new uint8[capacity];
  memcpy(newBuffer, buffer, length*sizeof(uint8));
  delete[] buffer;
  buffer = newBuffer;
}

void OArchive::dumpBuffer() const
{
  Debug(stringUtils_bufferToHex(buffer, length));
}

OArchive& OArchive::operator<<(const uint16& i)
{
  ensureSpace(sizeof(uint16));
  *reinterpret_cast<uint16*>(buffer+length) = ntohs(i);
  length += sizeof(uint16);
  return *this;
}

OArchive& OArchive::operator<<(const sint16& i)
{
  ensureSpace(sizeof(sint16));
  *reinterpret_cast<sint16*>(buffer+length) = ntohs(i);
  length += sizeof(sint16);
  return *this;
}

OArchive& OArchive::operator<<(const uint32& i)
{
  ensureSpace(sizeof(uint32));
  *reinterpret_cast<uint32*>(buffer+length) = ntohl(i);
  length += sizeof(uint32);
  return *this;
}

OArchive& OArchive::operator<<(const sint32& i)
{
  ensureSpace(sizeof(sint32));
  *reinterpret_cast<sint32*>(buffer+length) = ntohl(i);
  length += sizeof(sint32);
  return *this;
}

OArchive& OArchive::operator<<(const uint64& i)
{
  *this << uint32(i >> 32) << uint32(i);
  return *this;
}

OArchive& OArchive::operator<<(const sint64& i)
{
  *this << uint64(i);
  return *this;
}

OArchive& OArchive::operator<<(const double& d)
{
  *this << numToString(d);
  return *this;
}

OArchive& OArchive::operator<<(const String& s)
{
  size_t sLength = s.length();
  /** \todo Probably this should be an exception rather than a Fatal, but maybe
   * we should handle it altogether differently?
   */
  *this << static_cast<uint32>(sLength);
  ensureSpace(sLength);
  memcpy(buffer+length, s.c_str(), sLength);
  length += sLength;
  return *this;
}

OArchive& OArchive::operator<<(const OArchive& archive)
{
  size_t sLength = archive.length;
  *this << static_cast<uint32>(sLength);
  ensureSpace(sLength);
  memcpy(buffer+length, archive.getBytes(), sLength);
  length += sLength;
  return *this;
}

