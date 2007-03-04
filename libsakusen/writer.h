#ifndef LIBSAKUSEN__WRITER_H
#define LIBSAKUSEN__WRITER_H

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "oarchive.h"

namespace sakusen {

/** \brief Abstract base for something to which we can write (like a file) */
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
     * Throws exception on error */
    virtual void write(const uint8* buffer, size_t length) = 0;
    inline void write(const OArchive& archive) {
      write(archive.getBytes(), archive.getBytesLength());
    }
};

}

#endif // LIBSAKUSEN__WRITER_H

