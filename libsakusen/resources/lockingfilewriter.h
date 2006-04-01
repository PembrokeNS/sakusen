#ifndef LOCKINGFILEWRITER_H
#define LOCKINGFILEWRITER_H

#include "libsakusen-resources-global.h"
#include "oarchive.h"
#include "lockingfile.h"

namespace sakusen {
namespace resources {

class LockingFileWriter : public LockingFile {
  private:
    LockingFileWriter();
    LockingFileWriter(const LockingFileWriter&);
  public:
    LockingFileWriter(const String& fileName);
    virtual ~LockingFileWriter() {}
  protected:
    virtual short getLockType() const;
    virtual int open();
  public:
    bool write(const uint8* buffer, size_t length, bool block);
    inline bool write(const OArchive& archive, bool block) {
      return write(archive.getBytes(), archive.getBytesLength(), block);
    }
};

}}

#endif // LOCKINGFILEWRITER_H

