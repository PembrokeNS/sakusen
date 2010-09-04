#ifndef LIBSAKUSEN_RESOURCES__FILE_H
#define LIBSAKUSEN_RESOURCES__FILE_H

#include "libsakusen-global.h"

#include <boost/shared_array.hpp>
#include <boost/utility.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

namespace sakusen {
namespace resources {

/** \brief Class representing a file on disk with appropriate helper functions
 * 
 * \bug Methods marked as throwing FileIOExns (and maybe other methods), may
 * also throw std::ios_base::failure or boost::filesystem::filesystem_error. */
class File : private boost::noncopyable {
  protected:
    File(const boost::filesystem::path& filePath);
  public:
    virtual ~File();
  protected:
    boost::filesystem::path path;
    /** stream interface to the file (not necessarily open) */
    boost::filesystem::fstream stream;
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

#endif // LIBSAKUSEN_RESOURCES__FILE_H

