#include <sakusen/naivespatial.h>

#include <sakusen/serializationhandler.h>

using namespace std;

namespace sakusen {

void NaiveSpatial::add(const Ref<Bounded>& b)
{
  list.push_back(b);
  List::iterator i = list.end();
  --i;
  assert(!iteratorMap.count(b));
  iteratorMap[b] = i;
}

void NaiveSpatial::remove(const Ref<Bounded>& b)
{
  u_map<MaskedPtr<Bounded>, List::iterator>::type::iterator i =
    iteratorMap.find(b);
  assert(i != iteratorMap.end());
  list.erase(i->second);
  iteratorMap.erase(i);
}

ISpatial::Result NaiveSpatial::findIntersecting(
    const Box<sint32>& box,
    const GameObject filter
  ) const
{
  ISpatial::Result result;

  for (List::const_iterator i=list.begin(); i != list.end(); ++i) {
    if (((*i)->getObjectType() & filter) && (*i)->fastIntersection(box)) {
      result.push_back(*i);
    }
  }

  return result;
}

ISpatial::Result NaiveSpatial::findFastContaining(
    const Position& pos,
    const GameObject filter
  ) const
{
  ISpatial::Result result;

  for (List::const_iterator i=list.begin(); i != list.end(); ++i) {
    if (((*i)->getObjectType() & filter) && (*i)->fastContains(pos)) {
      result.push_back(*i);
    }
  }

  return result;
}

ISpatial::Result NaiveSpatial::findContaining(
    const Position& pos,
    const GameObject filter
  ) const
{
  ISpatial::Result result;

  for (List::const_iterator i=list.begin(); i != list.end(); ++i) {
    if (((*i)->getObjectType() & filter) && (*i)->contains(pos)) {
      result.push_back(*i);
    }
  }

  return result;
}

std::set<Intersection, LessThanIntersectionPosition>
NaiveSpatial::findIntersections(
    const Ray& ray,
    const double extent,
    const GameObject filter
  ) const
{
  std::set<Intersection, LessThanIntersectionPosition> result;

  for (List::const_iterator i=list.begin(); i != list.end(); ++i) {
    if (((*i)->getObjectType() & filter)) {
      double p1, p2;
      boost::tie(p1,p2) = (*i)->intersect(ray);
      if (extent >= p1) {
        if (p1 > 0) {
          result.insert(Intersection(*i, p1, false));
        }
        if (extent >= p2 && p2 > 0) {
          result.insert(Intersection(*i, p2, true));
        }
      }
    }
  }

  return result;
}

}

