#ifndef FILE_H
#define FILE_H

#include "libsakusen-global.h"

#include <boost/shared_array.hpp>
#include <boost/utility.hpp>

namespace sakusen {
namespace resources {

class File : boost::noncopyable {
  protected:
    File(const String& fileName);
  public:
    virtual ~File();
  protected:
    String fileName;
    FILE* stream; /**< file stream of file (NULL if not open) */
    int fd; /**< file descriptor of file (-1 if not open) */
    uint64 length; /**< The length of the file */
    /** true iff we know the length of the file (i.e. \a length is valid) */
    bool lengthIsKnown;
    
    virtual void open() = 0;
    int close();
  public:
    /** \brief Get length of file
     *
     * This attempts to get the length of the file.  Throws FileIOExn on error.
     */
    uint64 getLength();

    /** \brief Reads entire file into buffer
     *
     * This function attempts to read the entire file into the buffer.  If
     * successful, it returns the number of bytes read.  On error, it throws a
     * FileIOExn.  If \a length is insufficient to store the entire file, it
     * does nothing and returns 0.
     */
    size_t getWholeFile(uint8* buffer, size_t length);

    /** \brief Reads entire file into buffer
     *
     * This function attempts to read the entire file into the buffer.  If
     * successful, it returns the number of bytes read.  On error, it throws a
     * FileIOExn.  If \a length is insufficient to store the entire file, it
     * does nothing and returns 0.
     */
    inline size_t getWholeFile(
        const boost::shared_array<uint8>& buffer, size_t length
      ) {
      return getWholeFile(buffer.get(), length);
    }
};

}}

#endif // FILE_H

