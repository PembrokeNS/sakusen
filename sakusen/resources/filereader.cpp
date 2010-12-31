#include <sakusen/resources/filereader.h>

#include <sakusen/fileutils.h>
#include <sakusen/resources/fileioexn.h>

namespace sakusen {
namespace resources {

FileReader::FileReader(const boost::filesystem::path& filePath) :
  File(filePath)
{
}

/** \brief Open FileReader for reading
 *
 * Throws FileIOExn on error.
 */
void FileReader::open()
{
  stream.open(path, std::ios_base::binary | std::ios_base::in);
}

Buffer FileReader::getBuffer()
{
  uint64 length = getLength();
  /** \bug For the moment we have a length constraint to prevent excessive
   * memory allocation.  In the long run we probably want to make IArchive
   * abstract so that we can have a version that extracts data directly from
   * the file rather than copying it all over the place as we do at present */
  if (length > (1 << 20)) {
    SAKUSEN_FATAL("file size exceeded arbitrary limit");
  }

  size_t lengthAsSizeT = static_cast<size_t>(length);
  Buffer buffer(length);
  size_t bytesRead = getWholeFile(buffer.get(), length);
  if (lengthAsSizeT != bytesRead) {
    throw FileIOExn("error reading file");
  }
  return buffer;
}

String FileReader::humanReadableName() const
{
  return path.native_file_string();
}

boost::filesystem::path FileReader::asPath() const
{
  return path;
}

}}

