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
    sint64 length; /* The length of the file, or -1 if uncertain */
    
    short getLockType() const;
    int open();
  public:
    bool releaseLock();

    /* \brief Get length of file
     *
     * This attempts to get the length of the file.  Returns -1 on error.
     * \a block indicates whether a blocking call should be made if a lock must
     * be obtained.
     */
    sint64 getLength(bool block);

    /** \brief Reads entire file into buffer
     *
     * This function attempts to read the entire file into the buffer.  If
     * successful, it returns the number of bytes read.  On error, it returns
     * -1.  if \a length is insufficient to store the entire file, it does
     *  nothing and returns 0
     */
    ssize_t getWholeFile(uint8* buffer, ssize_t length, bool block);
};

}}

#endif // LOCKINGFILEREADER_H

