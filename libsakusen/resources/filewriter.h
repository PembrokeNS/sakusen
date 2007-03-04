#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "libsakusen-resources-global.h"
#include "oarchive.h"
#include "writer.h"
#include "file.h"

namespace sakusen {
namespace resources {

class FileWriter : public File, public Writer {
  public:
    FileWriter(const String& fileName);
    virtual ~FileWriter() {}
  protected:
    virtual void open();
  public:
    void write(const uint8* buffer, size_t length);
};

}}

#endif // FILEWRITER_H

