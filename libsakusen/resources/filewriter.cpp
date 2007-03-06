#include "filewriter.h"

#include "fileutils.h"
#include "fileioexn.h"

#include <fcntl.h>
#include <stdio.h>

using namespace sakusen::resources;

FileWriter::FileWriter(const String& name) :
  File(name)
{
}

void FileWriter::open()
{
  #ifdef _MSC_VER //Secure deprecation warning
    errno_t err;
    err = fopen_s(&stream, fileName.c_str(), "wb");
    if (err) {
      throw FileIOExn("fopen_s");
    }
    fd = _fileno(stream);
  #else
    stream = fopen(fileName.c_str(), "wb");
    if (stream == NULL) {
      throw FileIOExn("fopen");
    }
    fd = fileno(stream);
  #endif
  if (fd == -1) {
    stream = NULL;
    throw FileIOExn("fileno");
  }
}

void FileWriter::write(const uint8* buffer, size_t length)
{
  if (fd == -1) {
    open();
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
  size_t numBytes = fileUtils_write(fd, buffer, length);
  /*Debug("wrote " << numBytes << " bytes");*/
  if (size_t(numBytes) != length) {
    /*QDebug("write failed. tried to write "<<length<<" bytes, but wrote "<<numBytes<<" bytes.");*/
    throw FileIOExn("write");
  }
  return;
}

uint64 FileWriter::tell()
{
  if (fd == -1) {
    open();
  }

  sint64 result = NativeFtell64(stream);
  if (-1 == result) {
    throw FileIOExn("ftell");
  }

  return result;
}

