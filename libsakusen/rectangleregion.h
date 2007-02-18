#ifndef RECTANGLEREGION_H
#define RECTANGLEREGION_H

#include "region.h"
#include "world.h"
#include "rectangle.h"

namespace sakusen {

/** \brief Describes a region which is a rectangle in x-y and infinite in the
 * z-direction. */
template<typename T>
class RectangleRegion : public Region<T> {
  friend class Region<T>;
  public:
    RectangleRegion(const Rectangle<T>& r) : rectangle(r) {}
  private:
    Rectangle<T> rectangle;

    void storeData(OArchive&) const;
    static RectangleRegion<T>* loadNew(IArchive&);
  public:
    inline bool contains(const Point<T>& point) const;
    inline Point<T> truncateToFit(const Point<T>&) const;
    inline Point<T> getBestPosition() const;
    inline Rectangle<T> getBoundingRectangle() const { return rectangle; }
    
    RegionType getType() const { return regionType_rectangle; }
};

template<typename T>
inline bool RectangleRegion<T>::contains(const Point<T>& point) const {
  return rectangle.contains(point);
}

template<typename T>
inline Point<T> RectangleRegion<T>::truncateToFit(const Point<T>& p) const
{
  if (contains(p)) {
    return p;
  }
  Point<T> t(p);
  t.x = static_cast<T>(std::min(std::max(t.x, rectangle.getMinX()), static_cast<T>(rectangle.getMaxX())));
  t.y = static_cast<T>(std::min(std::max(t.y, rectangle.getMinY()), static_cast<T>(rectangle.getMaxY())));
  return t;
}

template<typename T>
inline Point<T> RectangleRegion<T>::getBestPosition() const
{
  return Point<T>(
      (rectangle.getMinX() + rectangle.getMaxX()) / 2,
      (rectangle.getMinY() + rectangle.getMaxY()) / 2,
      0
    );
}

template<typename T>
void RectangleRegion<T>::storeData(OArchive& archive) const
{
  rectangle.store(archive);
}

template<typename T>
RectangleRegion<T>* RectangleRegion<T>::loadNew(IArchive& archive)
{
  Rectangle<T> rectangle = Rectangle<T>::load(archive);
  return new RectangleRegion<T>(rectangle);
}

}

#endif // RECTANGLEREGION_H

