#include "oarchive.h"

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
  buffer = new uint8[copy.length];
  length = copy.length;
  capacity = length;
  memcpy(buffer, copy.buffer, length*sizeof(uint8));
  return *this;
}

void OArchive::expand()
{
  size_t newCapacity;
  if (capacity == 0) {
    newCapacity = 1;
  } else {
    newCapacity = capacity * 2;
  }
  uint8* newBuffer = new uint8[newCapacity];
  memcpy(newBuffer, buffer, length*sizeof(uint8));
  delete[] buffer;
  buffer = newBuffer;
  capacity = newCapacity;
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
  ensureSpace(sizeof(uint64));
  *reinterpret_cast<uint64*>(buffer+length) = ntohl(i);
  length += sizeof(uint64);
  return *this;
}

OArchive& OArchive::operator<<(const sint64& i)
{
  ensureSpace(sizeof(sint64));
  *reinterpret_cast<sint64*>(buffer+length) = ntohl(i);
  length += sizeof(sint64);
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

