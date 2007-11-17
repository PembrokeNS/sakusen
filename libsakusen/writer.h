#ifndef LIBSAKUSEN__WRITER_H
#define LIBSAKUSEN__WRITER_H

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "oarchive.h"

namespace sakusen {

/** \brief Abstract base for something to which we can write raw binary data
 * (like a file).
 *
 * The target of the Writer need not be seekable (it might be a network socket,
 * for example) but it does have to keep track of its position.
 */
class Writer : boost::noncopyable {
  public:
    typedef boost::shared_ptr<Writer> Ptr;
    typedef boost::shared_ptr<const Writer> ConstPtr;
  protected:
    Writer() {}
  public:
    virtual ~Writer() {}

    /** \brief Writes given data to target of Writer.
     *
     * Throws exception on error. */
    virtual void write(const uint8* buffer, size_t length) = 0;
    /** \brief Writes underlying data of Buffer to target of Writer.
     *
     * Throw exception on error. */
    inline void write(const Buffer& buffer) {
      write(buffer.getConst(), buffer.getSize());
    }
    /** \brief Writes underlying data of OArchive to target of Writer.
     *
     * Throw exception on error. */
    inline void write(const OArchive& archive) {
      write(archive.getBuffer());
    }

    /** \brief Report stream position (e.g. ftell) */
    virtual uint64 tell() = 0;
};

}

#endif // LIBSAKUSEN__WRITER_H

