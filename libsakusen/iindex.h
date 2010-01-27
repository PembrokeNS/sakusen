#ifndef LIBSAKUSEN__IINDEX_H
#define LIBSAKUSEN__IINDEX_H

#include "ref.h"

namespace sakusen {

/** \brief An interface to an index.
 *
 * This class provides a common interface to all types of index.  Indexes can
 * be registered with hash_lists, which call the add and remove methods
 * automatically, thus easing the use of such indexes.
 */
template<typename T>
class IIndex {
  public:
    typedef boost::shared_ptr<IIndex<T> > Ptr;
    typedef boost::shared_ptr<const IIndex<T> > ConstPtr;
  protected:
    IIndex() {}
    IIndex(const IIndex&) {}
  public:
    virtual ~IIndex() {}

    /** \brief Add a new entry to the index.
     *
     * If b is a valid Ref, adds it to the index; otherwise, does nothing.
     * \param[in] t Ref to the object to add.
     */
    virtual void add(const Ref<T>& t) = 0;

    /** \brief Remove an about-to-be-invalidated entry from the index.
     *
     * If b is a valid Ref, finds its entry in the index and removes it;
     * otherwise, does nothing.
     *
     * \param[in] t Ref to the object to add.
     */
    virtual void remove(const Ref<T>& t) = 0;
};

}

#endif // LIBSAKUSEN__IINDEX_H

