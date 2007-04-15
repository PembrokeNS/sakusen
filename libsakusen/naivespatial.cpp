#include "naivespatial.h"

#include "serializationhandler.h"

using namespace std;
using namespace __gnu_cxx;

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
  hash_map<MaskedPtr<Bounded>, List::iterator>::iterator i =
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
    if (((*i)->getType() & filter) && (*i)->fastIntersection(box)) {
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
    if (((*i)->getType() & filter) && (*i)->fastContains(pos)) {
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
    if (((*i)->getType() & filter) && (*i)->contains(pos)) {
      result.push_back(*i);
    }
  }

  return result;
}

std::map<double, Ref<Bounded> > NaiveSpatial::findIntersections(
    const Ray& ray,
    const double extent,
    const GameObject filter
  ) const
{
  std::map<double, Ref<Bounded> > result;

  for (List::const_iterator i=list.begin(); i != list.end(); ++i) {
    if (((*i)->getType() & filter)) {
      double parameter = (*i)->intersect(ray);
      if (extent >= parameter) {
        result.insert(make_pair(parameter, *i));
      }
    }
  }

  return result;
}

}

