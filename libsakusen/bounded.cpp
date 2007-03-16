#include "bounded.h"

namespace sakusen {

double Bounded::fastIntersect(const Ray& r) const {
  return r.intersectBox(getBoundingBox());
}

bool Bounded::fastIntersection(const Bounded& b) const {
  return !(getBoundingBox().intersection(b.getBoundingBox()).isEmpty());
}

bool Bounded::fastIntersection(const Box<sint32>& b) const {
  return !(getBoundingBox().intersection(b).isEmpty());
}

}
