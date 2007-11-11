#ifndef FILEREADER_H
#define FILEREADER_H

#include <boost/filesystem/path.hpp>

#include "file.h"

namespace sakusen {
namespace resources {

class FileReader : public File {
  public:
    FileReader(const boost::filesystem::path& filePath);
    ~FileReader() {}
  protected:
    void open();
  public:
};

}}

#endif // FILEREADER_H

