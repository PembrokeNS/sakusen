#ifndef LOCKINGFILE_H
#define LOCKINGFILE_H

#include "libsakusen-global.h"

#include <boost/shared_array.hpp>

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
    FILE* stream; /**< file stream of file (NULL if not open) */
    int fd; /**< file descriptor of file (-1 if not open) */
    bool haveLock;
    uint64 length; /**< The length of the file */
    bool lengthIsKnown; /**< true iff we know the length of the file (i.e. \a length is valid) */
    
    virtual short getLockType() const = 0;
    virtual int open() = 0;
    int close();
  public:
    /** \return true iff an error occurs (including EAGAIN when block==false) */
    virtual bool getLock(bool block) = 0; 

    /** \return true iff an error occurs */
    virtual bool releaseLock();

    /** \brief Get length of file
     *
     * This attempts to get the length of the file.  Returns -1 on error.
     * \param block indicates whether a blocking call should be made if a lock
     * must be obtained.
     */
    uint64 getLength(bool block);

    /** \brief Reads entire file into buffer
     *
     * This function attempts to read the entire file into the buffer.  If
     * successful, it returns the number of bytes read.  On error, it throws a
     * FileIOExn.  if \a length is insufficient to store the entire file, it does
     * nothing and returns 0.
     */
    size_t getWholeFile(uint8* buffer, size_t length, bool block);
    inline size_t getWholeFile(
        const boost::shared_array<uint8>& buffer, size_t length, bool block
      ) {
      return getWholeFile(buffer.get(), length, block);
    }
};

}}

#endif // LOCKINGFILE_H

