#ifndef LOCKINGFILEREADER_H
#define LOCKINGFILEREADER_H

#include "lockingfile.h"

namespace sakusen {
namespace resources {

class LockingFileReader : public LockingFile {
  private:
    LockingFileReader();
    LockingFileReader(const LockingFileReader&);
  public:
    LockingFileReader(const String& fileName);
    ~LockingFileReader() {}
  protected:
    bool getLock(bool block);
    short getLockType() const;
    int open();
  public:
};

}}

#endif // LOCKINGFILEREADER_H

