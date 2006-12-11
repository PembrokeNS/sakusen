#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "ref.h"

namespace sakusen {

/** \brief Class respresenting the intersection of a ray (or ballistic) with
 * some object */
class Intersection {
  public:
    Intersection(GameObject t, double p) :
      type(t),
      position(p),
      ref(NULL)
    {
      assert(type == gameObject_land || type == gameObject_water);
    }
    Intersection(const Intersection& copy) :
      type(copy.type),
      position(copy.position),
      ref((copy.ref == NULL) ? NULL : copy.ref->newCopy())
    {
    }
    Intersection& operator=(const Intersection& copy)
    {
      type = copy.type;
      position = copy.position;
      IRef* newRef = (copy.ref == NULL) ? NULL : copy.ref->newCopy();
      delete ref;
      ref = newRef;
      return *this;
    }
    ~Intersection()
    {
      delete ref;
      ref = NULL;
    }
  private:
    GameObject type;
    double position;
    IRef* ref;
  public:
    double getPosition() const { return position; }
    GameObject getType() const { return type; }

    template<typename T>
    const Ref<T>& getRef() const { return *dynamic_cast<Ref<T>*>(ref); }
};

class LessThanIntersectionPosition {
  public:
    bool operator()(const Intersection& left, const Intersection& right) const {
      return left.getPosition() < right.getPosition();
    }
};

}

#endif // INTERSECTION_H

