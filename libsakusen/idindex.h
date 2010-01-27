#ifndef LIBSAKUSEN__IDINDEX_H
#define LIBSAKUSEN__IDINDEX_H

#include<boost/call_traits.hpp>

#include "iindex.h"

namespace sakusen {

/** \brief An index based on some id which can be used as a hash_map key
 *
 * Three template arguments are required:
 * \li TId, the type of the id
 * \li T, the type of the object stored
 * \li Ider, the class of a function object which will extract the id of an
 *  object of type T.  Specifically, if t is of type Ref<T>,
 *  then \c Ider()(t)
 *  should return t's id.
 */
template<typename TId, typename T, typename Ider>
class IdIndex : public IIndex<T> {
  public:
    typedef boost::shared_ptr<IdIndex<TId, T, Ider> > Ptr;
    typedef boost::shared_ptr<const IdIndex<TId, T, Ider> > ConstPtr;
    
    IdIndex() {}
  private:
    typedef typename u_map<TId, Ref<T> >::type IdHashType;
    IdHashType idHash;
  public:
    void add(const Ref<T>&);
    void remove(const Ref<T>&);

    size_t count(typename boost::call_traits<TId>::param_type id) const;
    Ref<T> find(typename boost::call_traits<TId>::param_type id) const;
};

template<typename TId, typename T, typename Ider>
void IdIndex<TId, T, Ider>::add(const Ref<T>& t)
{
  TId id = Ider()(t);
  assert(idHash.count(id) == 0);
  idHash[id] = t;
}

template<typename TId, typename T, typename Ider>
void IdIndex<TId, T, Ider>::remove(const Ref<T>& t)
{
  TId id = Ider()(t);
  typename IdHashType::iterator it = idHash.find(id);
  assert(it != idHash.end());
  idHash.erase(it);
}

/** \brief Search for an object of the given id
 *
 * \param id id to search for
 * \return Ref to the object found, or an invalid Ref if none is found
 */
template<typename TId, typename T, typename Ider>
Ref<T> IdIndex<TId, T, Ider>::find(
    typename boost::call_traits<TId>::param_type id
  ) const
{
  typename IdHashType::const_iterator it = idHash.find(id);
  if (it == idHash.end()) {
    return Ref<T>();
  } else {
    return it->second;
  }
}

/** \brief Count objects of the given id
 *
 * \param id id to search for
 * \return 1 if an object with that key is found, or 0 if none is found
 */
template<typename TId, typename T, typename Ider>
size_t IdIndex<TId, T, Ider>::count(
    typename boost::call_traits<TId>::param_type id
  ) const
{
  return idHash.count(id);
}

}

#endif // LIBSAKUSEN__IDINDEX_H

