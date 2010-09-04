#include <sakusen/resources/file.h>
#include <sakusen/resources/fileutils.h>
#include <sakusen/comms/errorutils.h>
#include <sakusen/resources/fileioexn.h>

#include <cerrno>
#include <boost/filesystem/operations.hpp>

using namespace sakusen::comms;
using namespace sakusen::resources;

File::File(const boost::filesystem::path& f) :
  path(f),
  stream(),
  length(0),
  lengthIsKnown(false)
{
  /* Ask for exceptions to be throw on error */
  stream.exceptions(std::ios::badbit | std::ios::failbit);
}

File::~File()
{
}

int File::close()
{
  stream.close();
  if (stream.fail()) {
    return -1;
  }
  return 0;
}

uint64 File::getLength()
{
  try {
    length = boost::filesystem::file_size(path);
    lengthIsKnown = true;
    return length;
  } catch (...) {
    lengthIsKnown = false;
    throw;
  }
}

size_t File::getWholeFile(
    uint8* buffer,
    size_t bufferLen
  )
{
  getLength();
  /* If the above fails, it will throw an exn to the caller. */
  if (bufferLen < length) {
    SAKUSEN_DEBUG("buffer of insufficient size");
    /* do nothing, since we were asked for the *whole* file */
    return 0;
  }
  
  if (!stream.is_open()) {
    open();
  }
  stream.seekg(0, std::ios_base::beg);

  /** \todo It might be better to make stream a basic_fstream&lt;uint8&gt;,
   * then this reinterpret_cast is not needed. */
  stream.read(
      reinterpret_cast<char*>(buffer), static_cast<size_t>(length)
    );
  return stream.gcount();
}

