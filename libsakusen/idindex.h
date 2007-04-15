#ifndef IDINDEX_H
#define IDINDEX_H

#include<boost/call_traits.hpp>

#include "iindex.h"

namespace sakusen {

/** \brief An index based on some ID which can be used as a hash_map key
 *
 * Three template arguments are required:
 * \list
 * \li TID, the type of the ID
 * \li T, the type of the object stored
 * \li IDer, the class of a function object which will extract the ID of an
 *  object of type T.  Specifically, if t is of type Ref<T>,
 *  then \code IDer()(t)
 *  should return t's ID.
 * \endlist
 */
template<typename TID, typename T, typename IDer>
class IDIndex : public IIndex<T> {
  public:
    typedef boost::shared_ptr<IDIndex<TID, T, IDer> > Ptr;
    typedef boost::shared_ptr<const IDIndex<TID, T, IDer> > ConstPtr;
    
    IDIndex() {}
  private:
    __gnu_cxx::hash_map<TID, Ref<T> > idHash;
  public:
    void add(const Ref<T>&);
    void remove(const Ref<T>&);

    size_t count(typename boost::call_traits<TID>::param_type id);
    Ref<T> find(typename boost::call_traits<TID>::param_type id);
};

template<typename TID, typename T, typename IDer>
void IDIndex<TID, T, IDer>::add(const Ref<T>& t)
{
  TID id = IDer()(t);
  assert(idHash.count(id) == 0);
  idHash[id] = t;
}

template<typename TID, typename T, typename IDer>
void IDIndex<TID, T, IDer>::remove(const Ref<T>& t)
{
  TID id = IDer()(t);
  typename __gnu_cxx::hash_map<TID, Ref<T> >::iterator it = idHash.find(id);
  assert(it != idHash.end());
  idHash.erase(it);
}

/** \brief Search for an object of the given ID
 *
 * \param id ID to search for
 * \return Ref to the object found, or an invalid Ref if none is found
 */
template<typename TID, typename T, typename IDer>
Ref<T> IDIndex<TID, T, IDer>::find(
    typename boost::call_traits<TID>::param_type id
  )
{
  typename __gnu_cxx::hash_map<TID, Ref<T> >::iterator it = idHash.find(id);
  if (it == idHash.end()) {
    return Ref<T>();
  } else {
    return it->second;
  }
}

/** \brief Count objects of the given ID
 *
 * \param id ID to search for
 * \return 1 if an object with that key is found, or 0 if none is found
 */
template<typename TID, typename T, typename IDer>
size_t IDIndex<TID, T, IDer>::count(
    typename boost::call_traits<TID>::param_type id
  )
{
  typename __gnu_cxx::hash_map<TID, Ref<T> >::iterator it = idHash.find(id);
  return (it != idHash.end());
}

}

#endif // IDINDEX_H

