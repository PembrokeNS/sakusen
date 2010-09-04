#ifndef LIBSAKUSEN_RESOURCES__FILEREADER_H
#define LIBSAKUSEN_RESOURCES__FILEREADER_H

#include <boost/filesystem/path.hpp>

#include <sakusen/resources/file.h>
#include <sakusen/reader.h>

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

#endif // LIBSAKUSEN_RESOURCES__FILEREADER_H

