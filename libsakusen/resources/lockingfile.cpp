#include "lockingfile.h"
#include "fileutils.h"
#include "errorutils.h"
#include "fileioexn.h"

#include <fcntl.h>
#include <stdio.h>

#include <cerrno>

using namespace sakusen::comms;
using namespace sakusen::resources;

LockingFile::LockingFile(const String& f) :
  fileName(f),
  stream(NULL),
  fd(-1),
  haveLock(false),
  length(0),
  lengthIsKnown(false)
{
}

LockingFile::~LockingFile()
{
  if (fd != -1) {
    if (-1 == close()) {
      Debug("error closing file: " << errorUtils_parseErrno(errno));
    }
    fd = -1;
  }
}

int LockingFile::close()
{
  releaseLock(); /* Closing the file descriptor should release the lock, so
                    this is for paranoia's sake, (e.g. the close could fail)
                    */
  if (EOF == fclose(stream)) {
    stream = NULL;
    fd = -1;
    return -1;
  }
  stream = NULL;
  fd = -1;
  return 0;
}

bool LockingFile::releaseLock()
{
#if 0
  if (!haveLock) {
    return false;
  }
  if (-1 == open()) {
    return true;
  }
  int cmd = F_UNLCK;
  struct flock lock;
  lock.l_type = getLockType();
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0; /* Means to the end of the file */
  
  if (-1 == fcntl(fd, cmd, &lock)) {
    return true;
  }
  
#endif
  haveLock = false;
  lengthIsKnown = false; /* We no longer know the length once we release the lock */
  return false;
}

uint64 LockingFile::getLength(bool block)
{
  NativeStructStat statResult;
  if (lengthIsKnown) {
    return length;
  }
  if (getLock(block)) {
    throw new FileIOExn("getLock");
  }
  if (fstat(fd, &statResult) == -1) {
    throw new FileIOExn("fstat");
  }
  length = static_cast<uint64>(statResult.st_size);
  lengthIsKnown = true;
  return length;
}

size_t LockingFile::getWholeFile(
    uint8* buffer,
    size_t bufferLen,
    bool block
  )
{
  getLength(block);
  /* If the above fails, it will throw an exn to the caller. */
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

