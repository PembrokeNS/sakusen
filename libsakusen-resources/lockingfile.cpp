#include "lockingfile.h"

#include "errorutils.h"

#include <fcntl.h>

#include <cerrno>

LockingFile::LockingFile(const String& f) :
  fileName(f),
  stream(NULL),
  fd(-1),
  haveLock(false)
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

bool LockingFile::getLock(bool /*block*/)
{
  if (haveLock) {
    return false;
  }
  if (-1 == open()) {
    return true;
  }
  //int cmd = ( block ? F_SETLKW : F_SETLK );
  struct flock lock;
  lock.l_type = getLockType();
  lock.l_whence = SEEK_CUR;
  lock.l_start = 0;
  lock.l_len = 1; /* Means to the end of the file */
  
  /* FIXME: Locking Just Doesn't Work.  We omit and hope for the best */
  //if (-1 == fcntl(fd, cmd, &lock)/*lockf(fd, cmd, 0)*/) {
  //  return true;
  //}
  
  //haveLock = true;
  return false;
}

bool LockingFile::releaseLock()
{
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
  
  haveLock = false;
  return false;
}

