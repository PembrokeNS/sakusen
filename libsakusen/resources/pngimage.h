#ifndef PNGIMAGE_H
#define PNGIMAGE_H

#include "libsakusen-resources-global.h"

#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include "image.h"

namespace sakusen {
namespace resources {

class PngImageImpl;

class LIBSAKUSEN_RESOURCES_API PngImage : public Image {
  public:
    PngImage(const boost::filesystem::path& path);
    ~PngImage();
  private:
    boost::scoped_ptr<PngImageImpl> impl;
  public:
    void read(boost::multi_array<uint16, 2>& out) const;
};

}}

#endif // PNGIMAGE_H

