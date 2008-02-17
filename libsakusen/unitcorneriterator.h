#ifndef LIBSAKUSEN__UNITCORNERITERATOR_H
#define LIBSAKUSEN__UNITCORNERITERATOR_H

#include <boost/iterator/iterator_facade.hpp>

#include "point.h"

namespace sakusen {

/** \brief Iterator that can be used to get all the corners of a unit.
 *
 * Don't compare two iterators constructed from different units, the results
 * are not meaningful.
 * */
class UnitCornerIterator :
  public boost::iterator_facade<
      UnitCornerIterator,
      const Position,
      boost::bidirectional_traversal_tag
    > {
  friend class boost::iterator_core_access;
  public:
    /** \brief Constructs a past-the-end iterator. */
    UnitCornerIterator() : status(NULL), index(8) {}
    /** \brief Constructs an iterator pointing at a corner of the given unit.
     *
     * If the second optional parameter is false (the default) then all 8
     * corners will be visited as this iterator is incremented until it becomes
     * past-the-end.
     *
     * If it is true, then only the four bottom corners will be visited.
     */
    explicit UnitCornerIterator(const ICompleteUnit& u, bool bottom = false) :
      status(&u.getIStatus()),
      localPosition(u.getITypeData().getSize()),
      index(0)
    {
      if (bottom) {
        index = 4;
        localPosition.y *= -1;
        localPosition.z *= -1;
      }
      globalPosition = status->localToGlobal(localPosition);
    }
  private:
    const IUnitStatus* status;
    Position localPosition;
    Position globalPosition;
    unsigned int index;

    const Position& dereference() const {
      return globalPosition;
    }

    bool equal(const UnitCornerIterator& right) const {
      /* For speed, we just compare indicies, which works for all sane
       * applications, but might go a little strange if iterators for different
       * units are compared */
      return index == right.index;
    }

    void increment() {
      switch (index++) {
        case 0:
          localPosition.x *= -1;
          break;
        case 1:
          localPosition.y *= -1;
          break;
        case 2:
          localPosition.x *= -1;
          break;
        case 3:
          localPosition.z *= -1;
          break;
        case 4:
          localPosition.x *= -1;
          break;
        case 5:
          localPosition.y *= -1;
          break;
        case 6:
          localPosition.x *= -1;
          break;
        case 7:
          break;
        default:
          Fatal("incrementing iterator beyond end");
      }
      globalPosition = status->localToGlobal(localPosition);
    }

    void decrement() {
      switch (--index) {
        case 0:
          localPosition.x *= -1;
          break;
        case 1:
          localPosition.y *= -1;
          break;
        case 2:
          localPosition.x *= -1;
          break;
        case 3:
          localPosition.z *= -1;
          break;
        case 4:
          localPosition.x *= -1;
          break;
        case 5:
          localPosition.y *= -1;
          break;
        case 6:
          localPosition.x *= -1;
          break;
        case 7:
          break;
        default:
          Fatal("decrementing iterator beyond start");
      }
    }
};

}

#endif // LIBSAKUSEN__UNITCORNERITERATOR_H

