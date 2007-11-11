#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "libsakusen-resources-global.h"
#include "oarchive.h"
#include "writer.h"
#include "file.h"

namespace sakusen {
namespace resources {

/** \brief Implementation of Writer that stores its data in a file. */
class FileWriter : public File, public Writer {
  public:
    FileWriter(const boost::filesystem::path& filePath);
    virtual ~FileWriter() {}
  protected:
    virtual void open();
  public:
    void write(const uint8* buffer, size_t length);
    uint64 tell();
};

}}

#endif // FILEWRITER_H

