#include <sakusen/resources/filewriter.h>

#include <sakusen/resources/fileutils.h>
#include <sakusen/resources/fileioexn.h>

#include <fcntl.h>
#include <stdio.h>

using namespace sakusen::resources;

FileWriter::FileWriter(const boost::filesystem::path& name) :
  File(name)
{
}

void FileWriter::open()
{
  try {
    stream.open(
        path, std::ios_base::binary | std::ios_base::out
      );
  } catch (std::ios_base::failure&) {
    throw FileIOExn("open(\""+path.native_file_string()+"\")");
  }
}

void FileWriter::write(const uint8* buffer, size_t length)
{
  if (!stream.is_open()) {
    open();
  }
  /** \todo It might be better to make stream a basic_fstream&lt;uint8&gt;,
   * then this reinterpret_cast is not needed. */
  stream.write(reinterpret_cast<const char*>(buffer), length);
  /* Curiously, ostream::write seems to provide no means of determining how
   * much data was written in case of error.  Error should have thrown an
   * exception, though, so never mind... */
}

uint64 FileWriter::tell()
{
  sint64 result = stream.tellp();
  if (-1 == result) {
    throw FileIOExn("tell");
  }

  return result;
}

