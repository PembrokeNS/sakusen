#ifndef IHEIGHTFIELD_H
#define IHEIGHTFIELD_H

#include "ray.h"

namespace sakusen {

class LIBSAKUSEN_API IHeightfield {
  protected:
    IHeightfield() {}
    IHeightfield(const IHeightfield&) {}
    virtual ~IHeightfield() {}
  public:
    virtual sint32 getHeightAt(sint32 x, sint32 y) const = 0;
    /** \brief Get the height of the heightfield at a given position.
     *
     * Given a position in space, returns the
     * height on the vertical line through that position, in dex.
     *
     * \note \c p.z is irrelevant.
     */
    inline sint32 getHeightAt(const Position& p) const {
      return getHeightAt(p.x, p.y);
    }
    virtual inline sint32 getApproxHeightAt (sint32 x, sint32 y) const {
      return getHeightAt(x,y);
    }
    virtual inline sint32 getApproxHeightAt(const Position& p) const {
      return getHeightAt(p);
    }
    virtual sint32 getMaxHeightIn(const Rectangle<sint32>&) const = 0;
    virtual double intersectRay(const Ray&, double extent) const = 0;
};

}

#endif // IHEIGHTFIELD_H

