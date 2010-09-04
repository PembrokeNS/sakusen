#ifndef LIBSAKUSEN__IMAGE_H
#define LIBSAKUSEN__IMAGE_H

#include <sakusen/libsakusen-global.h>

#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>

namespace sakusen {

/** \brief Base class for images.
 */
class Image : private boost::noncopyable {
  public:
    typedef boost::shared_ptr<Image> Ptr;
    typedef boost::shared_ptr<const Image> ConstPtr;

    virtual ~Image() = 0;

    /** \brief Copies content of image into an array.
     *
     * Resizes the array \a out to the appropriate dimensions and copies the
     * image data into it.  The array is indexed by x and then y, so
     * \c out[width-1][height-1] contains the bottom right corner of the image.
     */
    virtual void read(boost::multi_array<uint16, 2>& out) const = 0;
};

inline Image::~Image() {}

}

#endif // LIBSAKUSEN__IMAGE_H

