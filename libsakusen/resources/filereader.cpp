#include "filereader.h"

#include "fileutils.h"
#include "fileioexn.h"

#include <fcntl.h>
#include <stdio.h>

using namespace sakusen::resources;

FileReader::FileReader(const String& fileName) :
  File(fileName)
{
}

/** \brief Open FileReader for reading
 *
 * Throws FileIOExn on error.
 */
void FileReader::open()
{
  #ifdef _MSC_VER //Secure deprecation warning
    errno_t err;
    err = fopen_s(&stream, fileName.c_str(), "rb");
    if (err) {
      throw FileIOExn("fopen_s");
    }
    fd = _fileno(stream);
  #else
    stream = fopen(fileName.c_str(), "rb");
    if (stream == NULL) {
      throw FileIOExn("fopen");
    }
    fd = fileno(stream);
  #endif
  /* This function should not fail, but on the off chance... */
  if (fd == -1) {
    stream = NULL;
    throw FileIOExn("fileno");
  }
}

