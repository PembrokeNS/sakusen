#ifndef PNGIMAGE_H
#define PNGIMAGE_H

#ifndef DISABLE_PNG

#include "libsakusen-resources-global.h"

#include <boost/scoped_ptr.hpp>

#include "image.h"

namespace sakusen {
namespace resources {

class PngImage_impl;

class LIBSAKUSEN_RESOURCES_API PngImage : public Image {
  public:
    PngImage(const String& fileName);
    ~PngImage();
  private:
    boost::scoped_ptr<PngImage_impl> impl;
  public:
    void read(boost::multi_array<uint16, 2>& out) const;
};

}}

#endif // DISABLE_PNG

#endif // PNGIMAGE_H

