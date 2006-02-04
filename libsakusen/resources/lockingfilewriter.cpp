#include "lockingfilewriter.h"

#include <fcntl.h>

using namespace sakusen::resources;

LockingFileWriter::LockingFileWriter(const String& name) :
  LockingFile(name)
{
}

short LockingFileWriter::getLockType() const
{
  return F_WRLCK;
  //return F_RDLCK;
}

int LockingFileWriter::open()
{
  stream = fopen(fileName.c_str(), "wb");
  if (stream == NULL) {
    return -1;
  }
  fd = fileno(stream);
  if (fd == -1) {
    stream = NULL;
    return -1;
  }
  return 0;
}

bool LockingFileWriter::write(const uint8* buffer, size_t length, bool block)
{
  if (getLock(block)) {
    return true;
  }
  /* The following is leftover debugging code which may again be useful for
   * someone
  String linkPath = "/proc/self/fd/" + numToString(fd);
  char buf[256];
  memset(buf, '\0', 256);
  int charsRead = readlink(linkPath.c_str(), buf, 256);
  buf[255]='\0';
  Debug("writing " << length << " bytes to file " << buf << " (" <<
      charsRead << ")");*/
  ssize_t numBytes = ::write(fd, buffer, length);
  /*Debug("wrote " << numBytes << " bytes");*/
  if (static_cast<size_t>(numBytes) != length) {
    return true;
  }
  return false;
}

