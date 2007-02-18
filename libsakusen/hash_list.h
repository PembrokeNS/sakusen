#ifndef HASH_LIST_H
#define HASH_LIST_H

#include <list>
#include "maskedptr.h"
#include "iref.h"
#include "ref.h"

namespace sakusen {

template<typename T>
class hash_list_iterator;

template<typename T>
class hash_list_const_iterator;

/** \brief A container which is much like a list, but with some hash-like
 * properties
 *
 * The primary hash-like property is that elements can be
 * looked up in O(1) (amortized) time from their pointer alone (Or,
 * equivalently, a Ref to the object).
 *
 * This is also an IRefContainer, so it provides its elements not by pointer or
 * by value but wrapped in a Ref<T>.  Note that such can be cast to Ref<U> for
 * other U using the members of Ref.
 *
 * Iterator invalidation semantics should be the same as those for a list (i.e.
 * all iterators remain valid until and unless the thing they point to is
 * erased).
 *
 * The ability to splice (as can be done with lists) is absent from hash_lists.
 */
template<typename T>
class hash_list : boost::noncopyable {
  friend class hash_list_iterator<T>;
  friend class hash_list_const_iterator<T>;
  public:
    typedef hash_list_iterator<T> iterator;
    typedef hash_list_const_iterator<T> const_iterator;
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
};

template <typename T>
bool operator==(
    const hash_list_const_iterator<T>& left,
    const hash_list_const_iterator<T>& right
  );

template <typename T>
bool operator!=(
    const hash_list_const_iterator<T>& left,
    const hash_list_const_iterator<T>& right
  );

template<typename T>
class hash_list_iterator {
  friend class hash_list<T>;
  friend class hash_list_const_iterator<T>;
  public:
    hash_list_iterator() :
      container(NULL),
      listIt()
    {}
  private:
    typedef typename hash_list<T>::List::iterator ListIt;

    hash_list_iterator(const ListIt& i, const hash_list<T>* c) :
      container(c),
      listIt(i)
    {}
    
    const hash_list<T>* container;
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

template<typename T>
class hash_list_const_iterator {
  friend bool operator==<T>(
      const hash_list_const_iterator& left,
      const hash_list_const_iterator& right
    );
  friend bool operator!=<T>(
      const hash_list_const_iterator& left,
      const hash_list_const_iterator& right
    );
  friend class hash_list<T>;
  private:
    typedef typename hash_list<T>::List::const_iterator ListIt;
    
    hash_list_const_iterator(const ListIt& i, const hash_list<T>* c) :
      container(c),
      listIt(i)
    {}

    const hash_list<T>* container;
    ListIt listIt;
  public:
    hash_list_const_iterator(const hash_list_iterator<T>&);
    Ref<const T> operator*() const;
    hash_list_const_iterator& operator++();
    hash_list_const_iterator operator++(int);
    hash_list_const_iterator& operator--();
    hash_list_const_iterator operator--(int);
};

template<typename T>
inline hash_list<T>::hash_list()
{
}

template<typename T>
inline hash_list<T>::~hash_list<T>()
{
  clear();
}

template<typename T>
inline typename hash_list<T>::iterator hash_list<T>::begin()
{
  return iterator(list.begin(), this);
}

template<typename T>
inline typename hash_list<T>::iterator hash_list<T>::end()
{
  return iterator(list.end(), this);
}

template<typename T>
inline typename hash_list<T>::const_iterator hash_list<T>::begin() const
{
  return const_iterator(list.begin(), this);
}

template<typename T>
inline typename hash_list<T>::const_iterator hash_list<T>::end() const
{
  return const_iterator(list.end(), this);
}

/** \brief Get the size of the hash_list
 *
 * \note Unlike std::list::size, this method is O(1)
 */
template<typename T>
inline typename hash_list<T>::size_type hash_list<T>::size() const
{
  return listIts.size();
}

template<typename T>
inline bool hash_list<T>::empty() const
{
  return list.empty();
}

template<typename T>
inline Ref<T> hash_list<T>::front()
{
  return Ref<T>(list.front());
}

template<typename T>
inline Ref<const T> hash_list<T>::front() const
{
  return Ref<const T>(list.front());
}

template<typename T>
inline Ref<T> hash_list<T>::back()
{
  return Ref<T>(list.back());
}

template<typename T>
inline Ref<const T> hash_list<T>::back() const
{
  return Ref<const T>(list.back());
}

template<typename T>
inline void hash_list<T>::push_front(T* item)
{
  insert(begin(), item);
}

template<typename T>
inline void hash_list<T>::push_front(const boost::shared_ptr<T>& item)
{
  insert(begin(), item);
}

template<typename T>
inline typename hash_list<T>::iterator hash_list<T>::push_back(T* item)
{
  return insert(end(), item);
}

template<typename T>
inline typename hash_list<T>::iterator hash_list<T>::push_back(
    const boost::shared_ptr<T>& item
  )
{
  return insert(end(), item);
}

template<typename T>
inline void hash_list<T>::pop_front()
{
  assert(!empty());
  erase(begin());
}

template<typename T>
inline void hash_list<T>::pop_back()
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
template<typename T>
typename hash_list<T>::iterator hash_list<T>::insert(
    iterator pos,
    T* itemPtr
  )
{
  boost::shared_ptr<T> item(itemPtr);
  return insert(pos, item);
}

template<typename T>
typename hash_list<T>::iterator hash_list<T>::insert(
    iterator pos,
    const boost::shared_ptr<T>& item
  )
{
  assert(pos.container == this);
  typename List::iterator it = pos.listIt;
  list.insert(it, item);
  --it; // Now it should point at the just-inserted item

  std::pair<typename ListIteratorHash::iterator, bool> result =
    listIts.insert(std::pair<MaskedPtr<T>, typename List::iterator>(item, it));

  if (!result.second) {
    throw std::logic_error("duplicate pointer inserted");
  }

  return iterator(it, this);
}

template<typename T> template<typename InputIterator>
inline void hash_list<T>::insert(
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

template<typename T>
typename hash_list<T>::iterator hash_list<T>::erase(iterator pos)
{
  assert(pos.container == this);
  iterator next = pos;
  ++next;
  boost::shared_ptr<T> ptr = *pos.listIt;
  listIts.erase(ptr);
  list.erase(pos.listIt);
  return next;
}

template<typename T>
inline typename hash_list<T>::iterator hash_list<T>::erase(MaskedPtr<T> item)
{
  iterator it = find(item);
  if (it == end())
    throw std::logic_error("no such item");
  return erase(it);
}

template<typename T>
inline typename hash_list<T>::iterator hash_list<T>::erase(
    iterator first,
    iterator last
  )
{
  while (first != last) {
    first = erase(first);
  }
  return last;
}

template<typename T>
inline void hash_list<T>::clear()
{
  erase(begin(), end());
  assert(list.empty());
  assert(listIts.empty());
}

template<typename T>
inline typename hash_list<T>::iterator hash_list<T>::find(MaskedPtr<T> tp)
{
  typename ListIteratorHash::iterator it = listIts.find(tp);
  if (it == listIts.end()) {
    return end();
  }
  return iterator(it->second, this);
}

template<typename T>
inline typename hash_list<T>::const_iterator hash_list<T>::find(
    MaskedPtr<T> tp
  ) const
{
  typename ListIteratorHash::const_iterator it = listIts.find(tp);
  if (it == listIts.end()) {
    return end();
  }
  return const_iterator(it->second, this);
}

template<typename T>
inline bool operator==(
    const hash_list_const_iterator<T>& left,
    const hash_list_const_iterator<T>& right
  )
{
  assert(left.container == right.container);
  return (left.listIt == right.listIt);
}

template<typename T>
inline bool operator!=(
    const hash_list_const_iterator<T>& left,
    const hash_list_const_iterator<T>& right
  )
{
  assert(left.container == right.container);
  return (left.listIt != right.listIt);
}

template<typename T>
inline bool hash_list_iterator<T>::operator==(const hash_list_iterator& right)
{
  assert(container == right.container);
  return (listIt == right.listIt);
}

template<typename T>
inline bool hash_list_iterator<T>::operator!=(const hash_list_iterator& right)
{
  assert(container == right.container);
  return (listIt != right.listIt);
}

template<typename T>
inline Ref<T> hash_list_iterator<T>::operator*() const
{
  return Ref<T>(*listIt);
}

template<typename T>
inline hash_list_iterator<T>& hash_list_iterator<T>::operator++()
{
  ++listIt;
  return *this;
}

template<typename T>
inline hash_list_iterator<T> hash_list_iterator<T>::operator++(int)
{
  hash_list_iterator copy(*this);
  ++*this;
  return copy;
}

template<typename T>
inline hash_list_iterator<T>& hash_list_iterator<T>::operator--()
{
  --listIt;
  return *this;
}

template<typename T>
inline hash_list_iterator<T> hash_list_iterator<T>::operator--(int)
{
  hash_list_iterator copy(*this);
  --*this;
  return copy;
}

template<typename T>
inline hash_list_const_iterator<T>::hash_list_const_iterator(
    const hash_list_iterator<T>& copy
  ) :
  container(copy.container),
  listIt(copy.listIt)
{
}

template<typename T>
inline Ref<const T> hash_list_const_iterator<T>::operator*() const
{
  return Ref<const T>(*listIt);
}

template<typename T>
inline hash_list_const_iterator<T>& hash_list_const_iterator<T>::operator++()
{
  ++listIt;
  return *this;
}

template<typename T>
inline hash_list_const_iterator<T> hash_list_const_iterator<T>::operator++(int)
{
  hash_list_const_iterator copy(*this);
  ++*this;
  return copy;
}

template<typename T>
inline hash_list_const_iterator<T>& hash_list_const_iterator<T>::operator--()
{
  --listIt;
  return *this;
}

template<typename T>
inline hash_list_const_iterator<T> hash_list_const_iterator<T>::operator--(int)
{
  hash_list_const_iterator copy(*this);
  --*this;
  return copy;
}

}

#endif // HASH_LIST_H

