#ifndef SPHEREREGION_H
#define SPHEREREGION_H

#include "libsakusen-global.h"
#include "region.h"
#include "world.h"
#include "mathsutils.h"

namespace sakusen {

/** \brief Describes a region which is spherical */
template<typename T>
class LIBSAKUSEN_API SphereRegion : public Region<T> {
  friend class Region<T>;
  /* Templatey typedefs with macros to spell them out in full for SWIG's
   * benefit
   */
#ifdef SWIG
#define UT IntMunger<T>::unsign
#define WideT IntMunger<T>::widest
#define UWideT IntMunger<IntMunger<T>::unsign>::widest
#else
    typedef typename IntMunger<T>::unsign UT;
    typedef typename IntMunger<T>::widest WideT;
    typedef typename IntMunger<UT>::widest UWideT;
#endif
  public:
    SphereRegion() : centre(), radius(0) {}
    SphereRegion(const Point<T>& c, UT r) :
      centre(c), radius(r) {}
  private:
    Point<T> centre;
    UT radius;

    void storeData(OArchive& archive) const
    {
      archive << centre << radius;
    }
    
    static SphereRegion<T>* loadNew(IArchive& archive)
    {
      Point<T> centre;
      UT radius;
      archive >> centre >> radius;
      return new SphereRegion<T>(centre, radius);
    }
  public:
    inline UWideT squareRadius() const { return UWideT(radius) * radius; }
    inline bool contains(const Point<sint16>& point) const;
    inline bool contains(const Point<sint32>& point) const;
    inline Point<sint16> truncateToFit(const Point<sint16>&) const;
    inline Point<sint32> truncateToFit(const Point<sint32>&) const;
    inline Point<T> getBestPosition() const { return centre; }
    inline Rectangle<T> getBoundingRectangle() const;
    inline Box<T> getBoundingBox() const;
    inline boost::tuple<double,double> intersect(const Ray& r) const;
    
    RegionType getType() const { return regionType_sphere; }
};

template<typename T>
inline bool SphereRegion<T>::contains(const Point<sint16>& point) const {
  return world->getMap()->getShortestDifference(point, centre).
    squareLength() < this->squareRadius();
}

template<typename T>
inline bool SphereRegion<T>::contains(const Point<sint32>& point) const {
  return world->getMap()->getShortestDifference(point, centre).
    squareLength() < this->squareRadius();
}

/** \bug This won't work if the sphere is not centred on the origin. I'm not
 * sure what to do about this. It looks like we would only ever want to use
 * this on the origin, and code to make it work anywhere would be just enough
 * of a speed penalty to make me unhappy.
 */
template<typename T>
inline Point<sint16> SphereRegion<T>::truncateToFit(
    const Point<sint16>& p
  ) const
{
  if (contains(p)) {
    return p;
  }
  return Point<sint16>(
      ((Point<double>(p) * radius) / p.length()).truncate()
    );
}

/** \bug This won't work if the sphere is not centred on the origin. I'm not
 * sure what to do about this. It looks like we would only ever want to use
 * this on the origin, and code to make it work anywhere would be just enough
 * of a speed penalty to make me unhappy.
 */
template<typename T>
inline Point<sint32> SphereRegion<T>::truncateToFit(
    const Point<sint32>& p
  ) const
{
  if (contains(p)) {
    return p;
  }
  return Point<sint32>(
      ((Point<double>(p) * radius) / p.length()).truncate()
    );
}

template<typename T>
inline Rectangle<T> SphereRegion<T>::getBoundingRectangle() const
{
  return Rectangle<T>(
      centre.x-radius,
      centre.y-radius,
      centre.x+radius,
      centre.y+radius
    );
}

template<typename T>
inline Box<T> SphereRegion<T>::getBoundingBox() const
{
  return Box<T>(
      Point<T>(
        centre.x-radius,
        centre.y-radius,
        centre.z-radius
      ),
      Point<T>(
        centre.x+radius,
        centre.y+radius,
        centre.z+radius
      )
    );
}

template<typename T>
inline boost::tuple<double,double> SphereRegion<T>::intersect(
    const Ray& r
  ) const
{
  const Position relativeOrigin = r.origin - centre;
  // Set up a quadratic to solve for the distance parameter
  double a = static_cast<double>(r.d.squareLength());
  double b = static_cast<double>(2*r.d.innerProduct(relativeOrigin));
  double c = static_cast<double>(relativeOrigin.squareLength());

  boost::tuple<double,double> result = mathsUtils_solveQuadratic(a, b, c);
  if (isnan(result.get<0>())) {
    result.get<0>() = result.get<1>() = std::numeric_limits<double>::infinity();
  }
  return result;
}

#ifdef _MSC_VER
template LIBSAKUSEN_API SphereRegion<sint16>;
template LIBSAKUSEN_API SphereRegion<sint32>;
#endif

}

#endif // SPHEREREGION_H

