#ifndef RECTANGLEREGIONDATA_H
#define RECTANGLEREGIONDATA_H

#include "regiondata.h"
#include "world.h"
#include "rectangle.h"

namespace sakusen {

/** \brief Describes a region which is a rectangle in x-y and infinite in the
 * z-direction. */
template<typename T>
class RectangleRegionData : public RegionData<T> {
  public:
    RectangleRegionData(const Rectangle<T>& r) : rectangle(r) {}
  private:
    Rectangle<T> rectangle;
  public:
    inline bool contains(const Point<T>& point) const;
    inline Point<T> truncateToFit(const Point<T>&) const;
    inline Point<T> getBestPosition() const;
    inline Rectangle<T> getBoundingRectangle() const { return rectangle; }
    
    RegionType getType() const { return regionType_rectangle; }
    RegionData<T>* newCopy() const { return new RectangleRegionData<T>(*this); }
    void store(OArchive&) const;
    static RectangleRegionData<T>* loadNew(IArchive&);
};

template<typename T>
inline bool RectangleRegionData<T>::contains(const Point<T>& point) const {
  return rectangle.contains(point);
}

template<typename T>
inline Point<T> RectangleRegionData<T>::truncateToFit(const Point<T>& p) const
{
  if (contains(p)) {
    return p;
  }
  Point<T> t(p);
  /* Do extra subtraction here because the top-right point is *not* inside the
   * Rectangle. This will need to be done differently if T = double. */
  t.x = static_cast<T>(std::min(std::max(t.x, rectangle.getMinX()), static_cast<T>(rectangle.getMaxX()-1)));
  t.y = static_cast<T>(std::min(std::max(t.y, rectangle.getMinY()), static_cast<T>(rectangle.getMaxY()-1)));
  return t;
}

template<typename T>
inline Point<T> RectangleRegionData<T>::getBestPosition() const
{
  return Point<T>(
      (rectangle.getMinX() + rectangle.getMaxX()) / 2,
      (rectangle.getMinY() + rectangle.getMaxY()) / 2,
      0
    );
}

template<typename T>
void RectangleRegionData<T>::store(OArchive& archive) const
{
  rectangle.store(archive);
}

template<typename T>
RectangleRegionData<T>* RectangleRegionData<T>::loadNew(IArchive& archive)
{
  Rectangle<T> rectangle = Rectangle<T>::load(archive);
  return new RectangleRegionData<T>(rectangle);
}

}

#endif // RECTANGLEREGIONDATA_H

