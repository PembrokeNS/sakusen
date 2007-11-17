#ifndef FILEREADER_H
#define FILEREADER_H

#include <boost/filesystem/path.hpp>

#include "file.h"
#include "reader.h"

namespace sakusen {
namespace resources {

class FileReader : public File, public Reader {
  public:
    FileReader(const boost::filesystem::path& filePath);
    ~FileReader() {}
  protected:
    void open();
  public:
    virtual Buffer getBuffer();
    virtual String humanReadableName() const;
    virtual boost::filesystem::path asPath() const;
};

}}

#endif // FILEREADER_H

