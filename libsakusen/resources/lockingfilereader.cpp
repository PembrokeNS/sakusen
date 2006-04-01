#include "lockingfilereader.h"

#include "fileioexn.h"
#include "fileutils.h"

#include <fcntl.h>
#include <stdio.h>

using namespace sakusen::resources;

LockingFileReader::LockingFileReader(const String& fileName) :
  LockingFile(fileName),
  length(-1)
{
}

short LockingFileReader::getLockType() const
{
#ifdef WIN32
  return 0;
#else
  return F_RDLCK;
#endif
}

int LockingFileReader::open()
{
  stream = fopen(fileName.c_str(), "rb");
  if (stream == NULL) {
    return -1;
  }
  fd = fileno(stream);
  /* This function should not fail, but on the off chance... */
  if (fd == -1) {
    stream = NULL;
    return -1;
  }
  return 0;
}

bool LockingFileReader::releaseLock()
{
  length = -1; /* We no longer know the length once we release the lock */
  return LockingFile::releaseLock();
}

sint64 LockingFileReader::getLength(bool block)
{
  if (length != -1) {
    return length;
  }
  if (getLock(block)) {
    return -1;
  }
  if (-1 == fseek(stream, 0, SEEK_END)) {
    return -1;
  }
  length = ftell(stream);
  return length;
}

size_t LockingFileReader::getWholeFile(
    uint8* buffer,
    size_t bufferLen,
    bool block
  )
{
  if (-1 == getLength(block)) {
    throw new FileIOExn("getLength");
  }
  if (bufferLen < length) {
    Debug("buffer of insufficient size");
    /* do nothing, since we were asked for the *whole* file */
    return 0;
  }
  
  if (-1 == fseek(stream, 0, SEEK_SET)) {
    throw new FileIOExn("fseek");
  }

  /* It's totally bizarre that this fflush should be necessary, but it is */
  if (EOF == fflush(stream)) {
    throw new FileIOExn("fflush");
  }

  return fileUtils_read(fd, buffer, length);
}

