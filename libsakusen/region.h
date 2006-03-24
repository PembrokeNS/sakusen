#ifndef REGION_H
#define REGION_H

#include "point.h"
#include "oarchive.h"
#include "iarchive.h"
#include "regiondata.h"

namespace sakusen {

template<typename T>
class LIBSAKUSEN_API Region {
  public:
    /** \brief Constructs region with given data.
     *
     * \param d data to use. */
    Region(const RegionData<T>& d) : data(d.newCopy()) {}
    /** \brief Constructs region with given data.
     *
     * \param d data to use.  Ownership of the pointer is transferred to the
     * newly constructed Region.  Must not be NULL */
    Region(RegionData<T>* d) : data(d) { assert(data != NULL); }
    Region(const Region<T>& copy) : data(copy.data->newCopy()) {}
    Region& operator=(const Region<T>& copy) {
      delete data;
      data = copy.data->newCopy();
      return *this;
    }
    ~Region() { delete data; }
  private:
    RegionData<T>* data; /* owned by this */
  public:
    /* game mechanics */
    inline bool contains(const Point<T>& point) const;
    inline bool contains(const IUnitStatus* unit) const;
    inline Point<T> truncateToFit(const Point<T>&) const;
    
    void store(OArchive&) const;
    static Region<T> load(IArchive&);
};

}
#endif // REGION_H

