#ifndef LIBSAKUSEN__INTERSECTION_H
#define LIBSAKUSEN__INTERSECTION_H

#include "ref.h"
#include "bounded.h"
#include "serializationhandler.h"

namespace sakusen {

/** \brief Class respresenting the intersection of a ray (or ballistic) with
 * some object */
class Intersection {
  public:
    /** \brief Constructs an intersection with land or water
     *
     * \param t The type of thing intersected.  Must be either gameObject_land
     *          or gameObject_water.
     * \param p Ray parameter of intersection.
     */
    Intersection(GameObject t, double p) :
      type(t),
      position(p),
      ref(),
      leaving(false)
    {
      assert(type == gameObject_land || type == gameObject_water);
    }
    /** \brief Constructs an intersection with a bounded game object.
     *
     * \param b The object intersected.  Must be either a unit or Effect.
     * \param p Ray parameter of the intersection.
     * \param l A bool indicating whether this intersection is marking the
     *          entry into the object (in which case false) or depature from it
     *          (in which case true). */
    Intersection(const Ref<Bounded>& b, double p, bool l) :
      type(b->getObjectType()),
      position(p),
      ref(b),
      leaving(l)
    {
      assert(type == gameObject_unit || type == gameObject_effect);
    }
  private:
    GameObject type;
    double position;
    Ref<Bounded> ref;
    bool leaving;
  public:
    /** \brief Get the ray parameter of this Intersection */
    double getPosition() const { return position; }
    /** \brief Get the type of object which this Intersection is with */
    GameObject getType() const { return type; }

    /** \brief Get a Ref to the thing intersected.
     *
     * Throws a bad cast exception if the template parameter is inappropriate.
     */
    template<typename T>
    Ref<T> getRef() const { return ref.dynamicCast<T>(); }

    /** \brief True iff the intersection is where the Ray is leaving the
     * object, rather than where the Ray is entering it. */
    bool isLeaving() const { return leaving; }
};

class LessThanIntersectionPosition {
  public:
    bool operator()(const Intersection& left, const Intersection& right) const {
      return left.getPosition() < right.getPosition();
    }
};

}

#endif // LIBSAKUSEN__INTERSECTION_H

