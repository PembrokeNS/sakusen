#ifndef LIBSAKUSEN__HASH_LIST_H
#define LIBSAKUSEN__HASH_LIST_H

#include <list>
#include <boost/variant.hpp>

#include "maskedptr.h"
#include "ref.h"
#include "iindex.h"

namespace sakusen {

#define SAKUSEN_HASH_LIST_LIMIT_INDEX_TYPES 2

#define SAKUSEN_HASH_LIST_ENUM_INDEXES( param )  \
    BOOST_PP_ENUM_PARAMS(SAKUSEN_HASH_LIST_LIMIT_INDEX_TYPES, param)

/* Following declarations mimic the ones in boost/variant/variant_fwd.hpp,
 * except that I've omitted the workaround in the hope that we won't actually
 * need it */

#define SAKUSEN_HASH_LIST_AUX_DECLARE_PARAMS_IMPL(z, N, T) \
  typename BOOST_PP_CAT(T,N) = void \
  /**/

#define SAKUSEN_HASH_LIST_DECLARE_PARAMS \
  BOOST_PP_ENUM( \
      SAKUSEN_HASH_LIST_LIMIT_INDEX_TYPES \
    , SAKUSEN_HASH_LIST_AUX_DECLARE_PARAMS_IMPL \
    , TIndex \
    ) \
  /**/

#define SAKUSEN_HASH_LIST_INDEX_ARGS_IMPL(z, N, T) \
  typename IIndex<BOOST_PP_CAT(T,N)>::Ptr \
  /**/

#define SAKUSEN_HASH_LIST_INDEX_ARGS \
  BOOST_PP_ENUM( \
      SAKUSEN_HASH_LIST_LIMIT_INDEX_TYPES \
    , SAKUSEN_HASH_LIST_INDEX_ARGS_IMPL \
    , TIndex \
    ) \
  /**/

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
class hash_list_iterator;

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
class hash_list_const_iterator;

/** \brief A container which is much like a list, but with some hash-like
 * properties
 *
 * The primary hash-like property is that elements can be
 * looked up in O(1) (amortized) time from their pointer alone (Or,
 * equivalently, a Ref to the object).
 *
 * This provides its elements not by pointer or
 * by value but wrapped in a Ref<T>.  Note that such can be cast to Ref<U> for
 * other U using the members of Ref.
 *
 * Iterator invalidation semantics should be the same as those for a list (i.e.
 * all iterators remain valid until and unless the thing they point to is
 * erased).
 *
 * The ability to splice (as can be done with lists) is absent from hash_lists.
 *
 * The first template parameter, T, is the type of object stored in the
 * hash_list.  Each subsequent template parameter declares a type which may be
 * the template parameter of an index which can be registered with the
 * hash_list.  No other types of index may be registered.  For example, \c
 * hash_list<UpdatedUnit,UpdateUnit,Bounded> declares a hash_list storing
 * UpdatedUnits and which accepts indexes of types IIndex<UpdatedUnit> and
 * IIndex<Bounded>.  The maximum number of such index types which may be used
 * is the compile-time constant SAKUSEN_HASH_LIST_LIMIT_INDEX_TYPES.
 */
template<typename T, SAKUSEN_HASH_LIST_DECLARE_PARAMS>
class hash_list : private boost::noncopyable {
  friend class hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>;
  friend
    class hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>;

  public:
    typedef hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>
      iterator;
    typedef hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>
      const_iterator;
    typedef size_t size_type;
    
    hash_list();
    ~hash_list();
  private:
    /** \brief Helper class for adding to indexes */
    class AddToIndexVisitor : public boost::static_visitor<void> {
      public:
        AddToIndexVisitor(const Ref<T>& t) : item(t) {}
      private:
        Ref<T> item;
      public:
        template<typename TIndex>
        void operator()(const boost::shared_ptr<IIndex<TIndex> >& index) {
          index->add(Ref<TIndex>(item));
        }
        void operator()(const boost::shared_ptr<IIndex<T> >& index) {
          index->add(item);
        }
    };

    /** \brief Helper class for removing from indexes */
    class RemoveFromIndexVisitor : public boost::static_visitor<void> {
      public:
        RemoveFromIndexVisitor(const Ref<T>& t) : item(t) {}
      private:
        Ref<T> item;
      public:
        template<typename TIndex>
        void operator()(const boost::shared_ptr<IIndex<TIndex> >& index) {
          index->remove(Ref<TIndex>(item));
        }
        void operator()(const boost::shared_ptr<IIndex<T> >& index) {
          index->remove(item);
        }
    };

    typedef std::list<boost::shared_ptr<T> > List;
    typedef typename u_map<MaskedPtr<T>, typename List::iterator>::type
      ListIteratorHash;
    
    List list;
    /** \brief A hash_map of iterators from the above list
     *
     * This allow a unit from the above list to
     * be erased in O(1) time from only its pointer.  It may seem perverse to
     * have the list at all, but that is important to guarantee iteration
     * ordering - different implementations of hash_map might iterate their
     * members in different orders, thus yielding non-reproducibility */
    ListIteratorHash listIts;

    typedef boost::variant<SAKUSEN_HASH_LIST_INDEX_ARGS> IndexVariant;
    /** \brief Indexes to be informed when objects are added or removed */
    std::vector<IndexVariant> indexes;
  public:
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    size_type size() const;
    bool empty() const;
    Ref<T> front();
    Ref<const T> front() const;
    Ref<T> back();
    Ref<const T> back() const;
    void push_front(T* item);
    void push_front(const boost::shared_ptr<T>& item);
    iterator push_back(T* item);
    iterator push_back(const boost::shared_ptr<T>& item);
    void pop_front();
    void pop_back();
    iterator insert(iterator pos, T* item);
    iterator insert(iterator pos, const boost::shared_ptr<T>& item);
    template<typename InputIterator>
    void insert(iterator pos, InputIterator first, InputIterator last);
    iterator erase(iterator pos);
    iterator erase(MaskedPtr<T>);
    iterator erase(iterator first, iterator last);
    void clear();
    iterator find(MaskedPtr<T>);
    const_iterator find(MaskedPtr<T>) const;

    template<typename TIndex>
    void registerIndex(const boost::shared_ptr<IIndex<TIndex> >&);
};

template <typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline bool operator==(
    const hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>& left,
    const hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>& right
  );

template <typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline bool operator!=(
    const hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>& left,
    const hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>& right
  );

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
class hash_list_iterator {
  friend class hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>;
  friend class hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>;
  public:
    hash_list_iterator() :
      listIt()
    {}
  private:
    typedef typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::List::iterator ListIt;

    explicit hash_list_iterator(const ListIt& i) :
      listIt(i)
    {}
    
    ListIt listIt;
  public:
    bool operator==(const hash_list_iterator&);
    bool operator!=(const hash_list_iterator&);
    Ref<T> operator*() const;
    hash_list_iterator& operator++();
    hash_list_iterator operator++(int);
    hash_list_iterator& operator--();
    hash_list_iterator operator--(int);
};

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
class hash_list_const_iterator {
  friend bool operator==<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>(
      const hash_list_const_iterator& left,
      const hash_list_const_iterator& right
    );
  friend bool operator!=<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>(
      const hash_list_const_iterator& left,
      const hash_list_const_iterator& right
    );
  friend class hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>;
  private:
    typedef typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::List::const_iterator ListIt;
    
    explicit hash_list_const_iterator(const ListIt& i) :
      listIt(i)
    {}

    ListIt listIt;
  public:
    hash_list_const_iterator(const hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>&);
    Ref<const T> operator*() const;
    hash_list_const_iterator& operator++();
    hash_list_const_iterator operator++(int);
    hash_list_const_iterator& operator--();
    hash_list_const_iterator operator--(int);
};

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::hash_list()
{
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::~hash_list()
{
  clear();
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::iterator hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::begin()
{
  return iterator(list.begin());
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::iterator hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::end()
{
  return iterator(list.end());
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::const_iterator
  hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::begin() const
{
  return const_iterator(list.begin());
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::const_iterator
  hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::end() const
{
  return const_iterator(list.end());
}

/** \brief Get the size of the hash_list
 *
 * \note Unlike std::list::size, this method is O(1)
 */
template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::size_type
  hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::size() const
{
  return listIts.size();
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline bool hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::empty() const
{
  return list.empty();
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline Ref<T> hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::front()
{
  return Ref<T>(list.front());
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline Ref<const T> hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::front() const
{
  return Ref<const T>(list.front());
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline Ref<T> hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::back()
{
  return Ref<T>(list.back());
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline Ref<const T> hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::back() const
{
  return Ref<const T>(list.back());
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline void hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::push_front(T* item)
{
  insert(begin(), item);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline void hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::push_front(
    const boost::shared_ptr<T>& item
  )
{
  insert(begin(), item);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::iterator
hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::push_back(
    T* item
  )
{
  return insert(end(), item);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::iterator
hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::push_back(
    const boost::shared_ptr<T>& item
  )
{
  return insert(end(), item);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline void hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::pop_front()
{
  assert(!empty());
  erase(begin());
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline void hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::pop_back()
{
  assert(!empty());
  iterator it = end();
  --it;
  erase(it);
}

/** \brief insert a new item into the container at a position specified by an
 * iterator
 *
 * \param pos     Insert item immediately before this iterator
 * \param itemPtr Item to insert (Ownership of pointer transferred to this)
 * \returns iterator pointing to the inserted item
 */
template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::iterator
hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::insert(
    iterator pos,
    T* itemPtr
  )
{
  boost::shared_ptr<T> item(itemPtr);
  return insert(pos, item);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::iterator
hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::insert(
    iterator pos,
    const boost::shared_ptr<T>& item
  )
{
  typename List::iterator it = pos.listIt;
  list.insert(it, item);
  --it; // Now it should point at the just-inserted item

  std::pair<typename ListIteratorHash::iterator, bool> result =
    listIts.insert(std::pair<MaskedPtr<T>, typename List::iterator>(item, it));

  if (!result.second) {
    throw std::logic_error("duplicate pointer inserted");
  }

  AddToIndexVisitor addVisitor = AddToIndexVisitor(Ref<T>(*it));
  std::for_each(
      indexes.begin(), indexes.end(), boost::apply_visitor(addVisitor)
    );

  return iterator(it);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
template<typename InputIterator>
inline void hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::insert(
    iterator pos,
    InputIterator first,
    InputIterator last
  )
{
  while (first != last) {
    insert(pos, *first);
    ++first;
  }
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::iterator
hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::erase(
    iterator pos
  )
{
  iterator next = pos;
  ++next;
  boost::shared_ptr<T> ptr = *pos.listIt;
  RemoveFromIndexVisitor removeVisitor = RemoveFromIndexVisitor(Ref<T>(ptr));
  for_each(
      indexes.begin(), indexes.end(), boost::apply_visitor(removeVisitor)
    );
  listIts.erase(ptr);
  list.erase(pos.listIt);
  return next;
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::iterator hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::erase(
    MaskedPtr<T> item
  )
{
  iterator it = find(item);
  if (it == end())
    throw std::logic_error("no such item");
  return erase(it);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::iterator hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::erase(
    iterator first,
    iterator last
  )
{
  while (first != last) {
    first = erase(first);
  }
  return last;
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline void hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::clear()
{
  erase(begin(), end());
  assert(list.empty());
  assert(listIts.empty());
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::iterator hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::find(
    MaskedPtr<T> tp
  )
{
  typename ListIteratorHash::iterator it = listIts.find(tp);
  if (it == listIts.end()) {
    return end();
  }
  return iterator(it->second);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline typename hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::const_iterator hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::find(
    MaskedPtr<T> tp
  ) const
{
  typename ListIteratorHash::const_iterator it = listIts.find(tp);
  if (it == listIts.end()) {
    return end();
  }
  return const_iterator(it->second);
}

/** \brief Register an index with the hash_list
 *
 * This function will attach an index to this hash_list, so that henceforth
 * that index can be used to look up entries in the hash_list.  The index must
 * implement IIndex<TIndex> where TIndex is one of the index types specified
 * amongst the type parameters of the hash_list.
 *
 * Indexes can be registered at any time; the hash_list need not be empty.
 */
template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
template<typename TIndex>
inline void hash_list<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::registerIndex(
    const boost::shared_ptr<IIndex<TIndex> >& index
  )
{
  indexes.push_back(index);
  /* Tell index about everything already in the hash_list */
  for (typename List::iterator i=list.begin(); i != list.end(); ++i) {
    index->add(Ref<TIndex>(*i));
  }
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline bool operator==(
    const hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>& left,
    const hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>& right
  )
{
  return (left.listIt == right.listIt);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline bool operator!=(
    const hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>& left,
    const hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>& right
  )
{
  return (left.listIt != right.listIt);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline bool
hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::operator==(
    const hash_list_iterator& right
  )
{
  return (listIt == right.listIt);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline bool
hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::operator!=(
    const hash_list_iterator& right
  )
{
  return (listIt != right.listIt);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline Ref<T>
hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::operator*() const
{
  return Ref<T>(*listIt);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>&
hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::operator++()
{
  ++listIt;
  return *this;
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>
hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::operator++(int)
{
  hash_list_iterator copy(*this);
  ++*this;
  return copy;
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>&
hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::operator--()
{
  --listIt;
  return *this;
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>
hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::operator--(int)
{
  hash_list_iterator copy(*this);
  --*this;
  return copy;
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::
hash_list_const_iterator(
    const hash_list_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>& copy
  ) :
  listIt(copy.listIt)
{
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline Ref<const T>
hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::operator*()
  const
{
  return Ref<const T>(*listIt);
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>&
  hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::operator++()
{
  ++listIt;
  return *this;
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>
  hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::operator++(int)
{
  hash_list_const_iterator copy(*this);
  ++*this;
  return copy;
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>&
  hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::operator--()
{
  --listIt;
  return *this;
}

template<typename T, SAKUSEN_HASH_LIST_ENUM_INDEXES(typename TIndex)>
inline hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>
  hash_list_const_iterator<T, SAKUSEN_HASH_LIST_ENUM_INDEXES(TIndex)>::operator--(int)
{
  hash_list_const_iterator copy(*this);
  --*this;
  return copy;
}

#undef SAKUSEN_HASH_LIST_INDEX_ARGS
#undef SAKUSEN_HASH_LIST_INDEX_ARGS_IMPL
#undef SAKUSEN_HASH_LIST_AUX_DECLARE_PARAMS_IMPL
#undef SAKUSEN_HASH_LIST_AUX_DECLARE_PARAMS

}

#endif // LIBSAKUSEN__HASH_LIST_H

