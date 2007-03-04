#include "file.h"
#include "fileutils.h"
#include "errorutils.h"
#include "fileioexn.h"

#include <fcntl.h>
#include <stdio.h>

#include <cerrno>

using namespace sakusen::comms;
using namespace sakusen::resources;

File::File(const String& f) :
  fileName(f),
  stream(NULL),
  fd(-1),
  length(0),
  lengthIsKnown(false)
{
}

File::~File()
{
  if (fd != -1) {
    if (-1 == close()) {
      Debug("error closing file: " << errorUtils_parseErrno(errno));
    }
    fd = -1;
  }
}

int File::close()
{
  if (EOF == fclose(stream)) {
    stream = NULL;
    fd = -1;
    return -1;
  }
  stream = NULL;
  fd = -1;
  return 0;
}

uint64 File::getLength()
{
  if (fd == -1) {
    open();
  }
  NativeStructStat statResult;
  if (lengthIsKnown) {
    return length;
  }
  if (NativeFstat(fd, &statResult) == -1) {
    throw FileIOExn("fstat");
  }
  length = static_cast<uint64>(statResult.st_size);
  lengthIsKnown = true;
  return length;
}

size_t File::getWholeFile(
    uint8* buffer,
    size_t bufferLen
  )
{
  getLength();
  /* If the above fails, it will throw an exn to the caller. */
  if (bufferLen < length) {
    Debug("buffer of insufficient size");
    /* do nothing, since we were asked for the *whole* file */
    return 0;
  }
  
  if (-1 == fseek(stream, 0, SEEK_SET)) {
    throw FileIOExn("fseek");
  }

  /* It's totally bizarre that this fflush should be necessary, but it is */
  if (EOF == fflush(stream)) {
    throw FileIOExn("fflush");
  }

  return fileUtils_read(fd, buffer, length);
}

