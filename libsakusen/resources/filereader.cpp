#include "filereader.h"

#include "fileutils.h"
#include "fileioexn.h"

#include <fcntl.h>
#include <stdio.h>

using namespace sakusen::resources;

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

