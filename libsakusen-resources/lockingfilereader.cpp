#include "lockingfilereader.h"

#include <fcntl.h>

LockingFileReader::LockingFileReader(const String& fileName) :
  LockingFile(fileName),
  length(-1)
{
}

short LockingFileReader::getLockType() const
{
  return F_RDLCK;
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

ssize_t LockingFileReader::getWholeFile(
    uint8* buffer,
    size_t bufferLen,
    bool block
  )
{
  if (-1 == getLength(block)) {
    return -1;
  }
  if (bufferLen < length) {
    Debug("buffer of insufficient size");
    /* do nothing, since we were asked for the *whole* file */
    return 0;
  }
  
  if (-1 == fseek(stream, 0, SEEK_SET)) {
    return -1;
  }

  /* It's totally bizarre that this fflush should be necessary, but it is */
  if (EOF == fflush(stream)) {
    return -1;
  }
  
  /* TODO: I note that if length>SSIZE_MAX, then this might cause problems (the
   * result is undefined), so we would need to read the file in chunks */
  return read(fd, buffer, length);
}

