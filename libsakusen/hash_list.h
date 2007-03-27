#ifndef HASH_LIST_H
#define HASH_LIST_H

#include <list>
#include "maskedptr.h"
#include "ref.h"
#include "iindex.h"

namespace sakusen {

template<typename T, typename TIndex>
class hash_list_iterator;

template<typename T, typename TIndex>
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
 */
template<typename T, typename TIndex = void>
class hash_list : boost::noncopyable {
  friend class hash_list_iterator<T, TIndex>;
  friend class hash_list_const_iterator<T, TIndex>;
  public:
    typedef hash_list_iterator<T, TIndex> iterator;
    typedef hash_list_const_iterator<T, TIndex> const_iterator;
    typedef size_t size_type;
    
    hash_list();
    ~hash_list();
  private:
    typedef std::list<boost::shared_ptr<T> > List;
    typedef __gnu_cxx::hash_map<MaskedPtr<T>, typename List::iterator>
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

    /** \brief Indexes to be informed when objects are added or removed */
    std::vector<typename IIndex<TIndex>::Ptr> indexes;
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

    void registerIndex(const typename IIndex<TIndex>::Ptr&);
};

template <typename T, typename TIndex>
bool operator==(
    const hash_list_const_iterator<T, TIndex>& left,
    const hash_list_const_iterator<T, TIndex>& right
  );

template <typename T, typename TIndex>
bool operator!=(
    const hash_list_const_iterator<T, TIndex>& left,
    const hash_list_const_iterator<T, TIndex>& right
  );

template<typename T, typename TIndex>
class hash_list_iterator {
  friend class hash_list<T, TIndex>;
  friend class hash_list_const_iterator<T, TIndex>;
  public:
    hash_list_iterator() :
      listIt()
    {}
  private:
    typedef typename hash_list<T, TIndex>::List::iterator ListIt;

    hash_list_iterator(const ListIt& i) :
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

template<typename T, typename TIndex>
class hash_list_const_iterator {
  friend bool operator==<T, TIndex>(
      const hash_list_const_iterator& left,
      const hash_list_const_iterator& right
    );
  friend bool operator!=<T, TIndex>(
      const hash_list_const_iterator& left,
      const hash_list_const_iterator& right
    );
  friend class hash_list<T, TIndex>;
  private:
    typedef typename hash_list<T, TIndex>::List::const_iterator ListIt;
    
    hash_list_const_iterator(const ListIt& i) :
      listIt(i)
    {}

    ListIt listIt;
  public:
    hash_list_const_iterator(const hash_list_iterator<T, TIndex>&);
    Ref<const T> operator*() const;
    hash_list_const_iterator& operator++();
    hash_list_const_iterator operator++(int);
    hash_list_const_iterator& operator--();
    hash_list_const_iterator operator--(int);
};

template<typename T, typename TIndex>
inline hash_list<T, TIndex>::hash_list()
{
}

template<typename T, typename TIndex>
inline hash_list<T, TIndex>::~hash_list<T, TIndex>()
{
  clear();
}

template<typename T, typename TIndex>
inline typename hash_list<T, TIndex>::iterator hash_list<T, TIndex>::begin()
{
  return iterator(list.begin());
}

template<typename T, typename TIndex>
inline typename hash_list<T, TIndex>::iterator hash_list<T, TIndex>::end()
{
  return iterator(list.end());
}

template<typename T, typename TIndex>
inline typename hash_list<T, TIndex>::const_iterator
  hash_list<T, TIndex>::begin() const
{
  return const_iterator(list.begin());
}

template<typename T, typename TIndex>
inline typename hash_list<T, TIndex>::const_iterator
  hash_list<T, TIndex>::end() const
{
  return const_iterator(list.end());
}

/** \brief Get the size of the hash_list
 *
 * \note Unlike std::list::size, this method is O(1)
 */
template<typename T, typename TIndex>
inline typename hash_list<T, TIndex>::size_type
  hash_list<T, TIndex>::size() const
{
  return listIts.size();
}

template<typename T, typename TIndex>
inline bool hash_list<T, TIndex>::empty() const
{
  return list.empty();
}

template<typename T, typename TIndex>
inline Ref<T> hash_list<T, TIndex>::front()
{
  return Ref<T>(list.front());
}

template<typename T, typename TIndex>
inline Ref<const T> hash_list<T, TIndex>::front() const
{
  return Ref<const T>(list.front());
}

template<typename T, typename TIndex>
inline Ref<T> hash_list<T, TIndex>::back()
{
  return Ref<T>(list.back());
}

template<typename T, typename TIndex>
inline Ref<const T> hash_list<T, TIndex>::back() const
{
  return Ref<const T>(list.back());
}

template<typename T, typename TIndex>
inline void hash_list<T, TIndex>::push_front(T* item)
{
  insert(begin(), item);
}

template<typename T, typename TIndex>
inline void hash_list<T, TIndex>::push_front(const boost::shared_ptr<T>& item)
{
  insert(begin(), item);
}

template<typename T, typename TIndex>
inline typename hash_list<T, TIndex>::iterator hash_list<T, TIndex>::push_back(
    T* item
  )
{
  return insert(end(), item);
}

template<typename T, typename TIndex>
inline typename hash_list<T, TIndex>::iterator hash_list<T, TIndex>::push_back(
    const boost::shared_ptr<T>& item
  )
{
  return insert(end(), item);
}

template<typename T, typename TIndex>
inline void hash_list<T, TIndex>::pop_front()
{
  assert(!empty());
  erase(begin());
}

template<typename T, typename TIndex>
inline void hash_list<T, TIndex>::pop_back()
{
  assert(!empty());
  iterator it = end();
  --it;
  erase(it);
}

/** \brief insert a new item into the container at a position specified by an
 * iterator
 *
 * \param pos  Insert item immediately before this iterator
 * \param item Item to insert (Ownership of pointer transferred to this)
 * \returns iterator pointing to the inserted item
 */
template<typename T, typename TIndex>
typename hash_list<T, TIndex>::iterator hash_list<T, TIndex>::insert(
    iterator pos,
    T* itemPtr
  )
{
  boost::shared_ptr<T> item(itemPtr);
  return insert(pos, item);
}

template<typename T, typename TIndex>
typename hash_list<T, TIndex>::iterator hash_list<T, TIndex>::insert(
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

  for (typename std::vector<typename IIndex<TIndex>::Ptr>::iterator i =
      indexes.begin(); i != indexes.end(); ++i) {
    (*i)->add(Ref<TIndex>(*it));
  }

  return iterator(it);
}

template<typename T, typename TIndex> template<typename InputIterator>
inline void hash_list<T, TIndex>::insert(
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

template<typename T, typename TIndex>
typename hash_list<T, TIndex>::iterator hash_list<T, TIndex>::erase(
    iterator pos
  )
{
  iterator next = pos;
  ++next;
  boost::shared_ptr<T> ptr = *pos.listIt;
  for (typename std::vector<typename IIndex<TIndex>::Ptr>::iterator i =
      indexes.begin(); i != indexes.end(); ++i) {
    (*i)->remove(Ref<TIndex>(ptr));
  }
  listIts.erase(ptr);
  list.erase(pos.listIt);
  return next;
}

template<typename T, typename TIndex>
inline typename hash_list<T, TIndex>::iterator hash_list<T, TIndex>::erase(
    MaskedPtr<T> item
  )
{
  iterator it = find(item);
  if (it == end())
    throw std::logic_error("no such item");
  return erase(it);
}

template<typename T, typename TIndex>
inline typename hash_list<T, TIndex>::iterator hash_list<T, TIndex>::erase(
    iterator first,
    iterator last
  )
{
  while (first != last) {
    first = erase(first);
  }
  return last;
}

template<typename T, typename TIndex>
inline void hash_list<T, TIndex>::clear()
{
  erase(begin(), end());
  assert(list.empty());
  assert(listIts.empty());
}

template<typename T, typename TIndex>
inline typename hash_list<T, TIndex>::iterator hash_list<T, TIndex>::find(
    MaskedPtr<T> tp
  )
{
  typename ListIteratorHash::iterator it = listIts.find(tp);
  if (it == listIts.end()) {
    return end();
  }
  return iterator(it->second);
}

template<typename T, typename TIndex>
inline typename hash_list<T, TIndex>::const_iterator hash_list<T, TIndex>::find(
    MaskedPtr<T> tp
  ) const
{
  typename ListIteratorHash::const_iterator it = listIts.find(tp);
  if (it == listIts.end()) {
    return end();
  }
  return const_iterator(it->second);
}

template<typename T, typename TIndex>
inline void hash_list<T, TIndex>::registerIndex(
    const typename IIndex<TIndex>::Ptr& index
  )
{
  indexes.push_back(index);
  /* Tell index about everything already in the hash_list */
  for (typename List::iterator i=list.begin(); i != list.end(); ++i) {
    index->add(Ref<TIndex>(*i));
  }
}

template<typename T, typename TIndex>
inline bool operator==(
    const hash_list_const_iterator<T, TIndex>& left,
    const hash_list_const_iterator<T, TIndex>& right
  )
{
  return (left.listIt == right.listIt);
}

template<typename T, typename TIndex>
inline bool operator!=(
    const hash_list_const_iterator<T, TIndex>& left,
    const hash_list_const_iterator<T, TIndex>& right
  )
{
  return (left.listIt != right.listIt);
}

template<typename T, typename TIndex>
inline bool hash_list_iterator<T, TIndex>::operator==(const hash_list_iterator& right)
{
  return (listIt == right.listIt);
}

template<typename T, typename TIndex>
inline bool hash_list_iterator<T, TIndex>::operator!=(const hash_list_iterator& right)
{
  return (listIt != right.listIt);
}

template<typename T, typename TIndex>
inline Ref<T> hash_list_iterator<T, TIndex>::operator*() const
{
  return Ref<T>(*listIt);
}

template<typename T, typename TIndex>
inline hash_list_iterator<T, TIndex>& hash_list_iterator<T, TIndex>::operator++()
{
  ++listIt;
  return *this;
}

template<typename T, typename TIndex>
inline hash_list_iterator<T, TIndex> hash_list_iterator<T, TIndex>::operator++(int)
{
  hash_list_iterator copy(*this);
  ++*this;
  return copy;
}

template<typename T, typename TIndex>
inline hash_list_iterator<T, TIndex>& hash_list_iterator<T, TIndex>::operator--()
{
  --listIt;
  return *this;
}

template<typename T, typename TIndex>
inline hash_list_iterator<T, TIndex> hash_list_iterator<T, TIndex>::operator--(int)
{
  hash_list_iterator copy(*this);
  --*this;
  return copy;
}

template<typename T, typename TIndex>
inline hash_list_const_iterator<T, TIndex>::hash_list_const_iterator(
    const hash_list_iterator<T, TIndex>& copy
  ) :
  listIt(copy.listIt)
{
}

template<typename T, typename TIndex>
inline Ref<const T> hash_list_const_iterator<T, TIndex>::operator*() const
{
  return Ref<const T>(*listIt);
}

template<typename T, typename TIndex>
inline hash_list_const_iterator<T, TIndex>&
  hash_list_const_iterator<T, TIndex>::operator++()
{
  ++listIt;
  return *this;
}

template<typename T, typename TIndex>
inline hash_list_const_iterator<T, TIndex>
  hash_list_const_iterator<T, TIndex>::operator++(int)
{
  hash_list_const_iterator copy(*this);
  ++*this;
  return copy;
}

template<typename T, typename TIndex>
inline hash_list_const_iterator<T, TIndex>&
  hash_list_const_iterator<T, TIndex>::operator--()
{
  --listIt;
  return *this;
}

template<typename T, typename TIndex>
inline hash_list_const_iterator<T, TIndex>
  hash_list_const_iterator<T, TIndex>::operator--(int)
{
  hash_list_const_iterator copy(*this);
  --*this;
  return copy;
}

}

#endif // HASH_LIST_H

