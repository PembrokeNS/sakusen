#ifndef INTERSECTION_H
#define INTERSECTION_H

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
     *          or gameObject_water
     * \param p Ray parameter of intersection.
     */
    Intersection(GameObject t, double p) :
      type(t),
      position(p),
      ref()
    {
      assert(type == gameObject_land || type == gameObject_water);
    }
    /** \brief Constructs an intersection with a bounded game object
     *
     * \param b The object intersected.  Must be either a unit or Effect
     * \param p Ray parameter of the intersection */
    Intersection(const Ref<Bounded>& b, double p) :
      type(b->getObjectType()),
      position(p),
      ref(b)
    {
      assert(type == gameObject_unit || type == gameObject_effect);
    }
  private:
    GameObject type;
    double position;
    Ref<Bounded> ref;
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
};

class LessThanIntersectionPosition {
  public:
    bool operator()(const Intersection& left, const Intersection& right) const {
      return left.getPosition() < right.getPosition();
    }
};

}

#endif // INTERSECTION_H

