#ifndef LOCKINGFILE_H
#define LOCKINGFILE_H

#include "libsakusen-global.h"

namespace sakusen {
namespace resources {

class LockingFile {
  private:
    LockingFile();
    LockingFile(const LockingFile&);
  protected:
    LockingFile(const String& fileName);
  public:
    virtual ~LockingFile();
  protected:
    String fileName;
    FILE* stream; /* file stream o file (NULL if not open) */
    int fd; /* file descriptor of file (-1 if not open) */
    bool haveLock;
    
    virtual short getLockType() const = 0;
    virtual int open() = 0;
    int close();
  public:
    virtual bool getLock(bool block); 
      /* Returns true iff an error occurs (including EAGAIN when block==false)
       * */
    virtual bool releaseLock();
      /* Returns true iff an error occurs */
};

}}

#endif // LOCKINGFILE_H

