#ifndef LIBSAKUSEN__RECTANGLEREGION_H
#define LIBSAKUSEN__RECTANGLEREGION_H

#include <sakusen/region.h>
#include <sakusen/world.h>
#include <sakusen/rectangle.h>

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
    inline bool contains(const Point<sint16>& point) const;
    inline bool contains(const Point<sint32>& point) const;
    template<typename U>
    inline Point<U> truncateToFitTemplate(const Point<U>&) const;
    inline Point<sint16> truncateToFit(const Point<sint16>&) const;
    inline Point<sint32> truncateToFit(const Point<sint32>&) const;
    inline Point<T> getBestPosition() const;
    inline Rectangle<T> getBoundingRectangle() const { return rectangle; }
    inline Box<sint32> getBoundingBox() const;
    inline boost::tuple<double,double> intersect(const Ray& r) const;
    
    RegionType getType() const { return regionType_rectangle; }
};

template<typename T>
inline bool RectangleRegion<T>::contains(const Point<sint16>& point) const {
  return rectangle.contains(point);
}

template<typename T>
inline bool RectangleRegion<T>::contains(const Point<sint32>& point) const {
  return rectangle.contains(point);
}

template<typename T>
template<typename U>
inline Point<U> RectangleRegion<T>::truncateToFitTemplate(const Point<U>& p) const
{
  if (contains(p)) {
    return p;
  }
  Point<U> t(p);
  /** \bug Using std::min/max&lt;sint32&gt; may not always suffice */
  t.x = static_cast<U>(std::min<sint32>(
        std::max<sint32>(t.x, rectangle.getMinX()), rectangle.getMaxX()
      ));
  t.y = static_cast<U>(std::min<sint32>(
        std::max<sint32>(t.y, rectangle.getMinY()), rectangle.getMaxY()
      ));
  return t;
}

template<typename T>
inline Point<sint16> RectangleRegion<T>::truncateToFit(const Point<sint16>& p) const
{
  return truncateToFitTemplate(p);
}

template<typename T>
inline Point<sint32> RectangleRegion<T>::truncateToFit(const Point<sint32>& p) const
{
  return truncateToFitTemplate(p);
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
inline Box<sint32> RectangleRegion<T>::getBoundingBox() const
{
  return Box<sint32>(
      Point<T>(
        rectangle.getMinX(),
        rectangle.getMinY(),
        bottomNumber<T>()
      ),
      Point<T>(
        rectangle.getMaxX(),
        rectangle.getMaxY(),
        topNumber<T>()
      )
    );
}

template<typename T>
boost::tuple<double,double> RectangleRegion<T>::intersect(const Ray& r) const
{
  /** \todo It might be worth writing a proper ray-rectangle intersection
   * routine, but probably not */
  assert(std::numeric_limits<T>::is_integer && sizeof(T) <= sizeof(sint32));
  return r.intersectBox(
      Box<sint32>(
        Point<sint32>(
          rectangle.getMinX(),
          rectangle.getMinY(),
          bottomNumber<sint32>()
        ),
        Point<sint32>(
          rectangle.getMaxX(),
          rectangle.getMaxY(),
          topNumber<sint32>()
        )
      )
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

#endif // LIBSAKUSEN__RECTANGLEREGION_H

