#ifndef NAIVESPATIAL_H
#define NAIVESPATIAL_H

#include "ispatial.h"

namespace sakusen {
/** \brief Naive implementation of spatial index
 *
 * This simple implementation of a spatial index is intended to serve as a
 * reference against which more efficient indices can be compared */
class NaiveSpatial : public ISpatial {
  private:
    typedef std::list<Ref<Bounded> > List;
    List list;
    __gnu_cxx::hash_map<MaskedPtr<Bounded>, List::iterator> iteratorMap;
  public:
    void add(const Ref<Bounded>&);
    void remove(const Ref<Bounded>&);

    Result findIntersecting(
        const Box<sint32>&,
        const GameObject filter
      );
};

}

#endif // NAIVESPATIAL_H

