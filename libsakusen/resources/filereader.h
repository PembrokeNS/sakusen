#ifndef FILEREADER_H
#define FILEREADER_H

#include "file.h"

namespace sakusen {
namespace resources {

class FileReader : public File {
  public:
    FileReader(const String& fileName);
    ~FileReader() {}
  protected:
    void open();
  public:
};

}}

#endif // FILEREADER_H

