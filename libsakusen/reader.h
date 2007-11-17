#ifndef LIBSAKUSEN__READER_H
#define LIBSAKUSEN__READER_H

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "buffer.h"

namespace sakusen {

/** \brief Abstract base for something from which we can read raw binary data
 * (like a file).
 */
class Reader : boost::noncopyable {
  public:
    typedef boost::shared_ptr<Reader> Ptr;
    typedef boost::shared_ptr<const Reader> ConstPtr;
  protected:
    Reader() {}
  public:
    virtual ~Reader() = 0;
    /** \brief Get the entire content of the Reader into a buffer of bytes in
     * one go. 
     *
     * Throws exception on error. */
    virtual Buffer getBuffer() = 0;
};

inline Reader::~Reader() {}

}

#endif // LIBSAKUSEN__READER_H

