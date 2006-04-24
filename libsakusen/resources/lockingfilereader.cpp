#include "lockingfilereader.h"

#include "fileutils.h"

#include <fcntl.h>
#include <stdio.h>

using namespace sakusen::resources;

LockingFileReader::LockingFileReader(const String& fileName) :
  LockingFile(fileName)
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

bool LockingFileReader::getLock(bool /*block*/)
{
  if (haveLock) {
    return false;
  }
  if (-1 == open()) {
    return true;
  }
#if 0
  /* FIXME: Locking Just Doesn't Work.  We omit and hope for the best */
  struct flock lock;
  lock.l_type = getLockType();
  lock.l_whence = SEEK_CUR;
  lock.l_start = 0;
  lock.l_len = 1; /* Means to the end of the file */
  
  //if (-1 == fcntl(fd, cmd, &lock)/*lockf(fd, cmd, 0)*/) {
  //  return true;
  //}
  
  //haveLock = true;
#endif
  return false;
}

int LockingFileReader::open()
{
  #ifdef _MSC_VER //Secure deprecation warning
    errno_t err;
    err = fopen_s(&stream, fileName.c_str(), "rb");
    if (err) {
      return -1;
    }
    fd = _fileno(stream);
  #else
    stream = fopen(fileName.c_str(), "rb");
    if (stream == NULL) {
      return -1;
    }
    fd = fileno(stream);
  #endif
  /* This function should not fail, but on the off chance... */
  if (fd == -1) {
    stream = NULL;
    return -1;
  }

  return 0;
}

