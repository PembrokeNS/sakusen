#include <sakusen/bounded.h>

#include <sakusen/ray.h>

using namespace std;

namespace sakusen {

boost::tuple<double,double> Bounded::fastIntersect(const Ray& r) const {
  return r.intersectBox(getBoundingBox());
}

bool Bounded::fastContains(const Position& p) const {
  return getBoundingBox().contains(p);
}

bool Bounded::fastIntersection(const Bounded& b) const {
  return !(getBoundingBox().intersection(b.getBoundingBox()).isEmpty());
}

bool Bounded::fastIntersection(const Box<sint32>& b) const {
  return !(getBoundingBox().intersection(b).isEmpty());
}

}
