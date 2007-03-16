#include "naivespatial.h"

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
  )
{
  ISpatial::Result result;

  for (List::iterator i=list.begin(); i != list.end(); ++i) {
    if (((*i)->getType() & filter) && (*i)->fastIntersection(box)) {
      result.push_back(*i);
    }
  }

  return result;
}

}

