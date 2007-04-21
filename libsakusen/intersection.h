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
    Intersection(GameObject t, double p) :
      type(t),
      position(p),
      ref()
    {
      assert(type == gameObject_land || type == gameObject_water);
    }
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
    double getPosition() const { return position; }
    GameObject getType() const { return type; }

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

