#ifndef LIBSAKUSEN_RESOURCES__FILEWRITER_H
#define LIBSAKUSEN_RESOURCES__FILEWRITER_H

#include <sakusen/resources/libsakusen-resources-global.h>
#include <sakusen/oarchive.h>
#include <sakusen/writer.h>
#include <sakusen/resources/file.h>

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

#endif // LIBSAKUSEN_RESOURCES__FILEWRITER_H

