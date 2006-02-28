#ifndef RECTANGLEREGIONDATA_H
#define RECTANGLEREGIONDATA_H

#include "regiondata.h"
#include "world.h"

namespace sakusen {

/** \brief Describes a region which is a rectangle in x-y and infinite in teh
 * z-direction. */
template<typename T>
class RectangleRegionData : public RegionData<T> {
  public:
    RectangleRegionData(const Rectangle& r) : rectangle(r) {}
  private:
    Rectangle rectangle;
  public:
    inline bool contains(const Point<T>& point) const;
    inline Point<T> truncateToFit(const Point<T>&) const;
    
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
  t.x = std::min(std::max(t.x, rectangle.x), rectangle.getMaxX());
  t.y = std::min(std::max(t.y, rectangle.y), rectangle.getMaxY());
  return t;
}

template<typename T>
void RectangleRegionData<T>::store(OArchive& archive) const
{
  archive << rectangle;
}

template<typename T>
SphereRegionData<T>* SphereRegionData<T>::loadNew(IArchive& archive)
{
  Rectangle rectangle;
  archive >> rectangle;
  return new RectangleRegionData<T>(rectangle);
}

}

#endif // RECTANGLEREGIONDATA_H

