#include "ui/surface.h"

namespace tedomari {
namespace ui {

Surface::Surface(uint16 w, uint16 h) :
  /** \bug Hard-coded 4 bytes per pixel */
  buffer(new uint8[w*h*4]),
  width(w),
  height(h)
{}

Surface::~Surface()
{
  delete[] buffer;
  buffer = NULL;
}

}}

