#ifndef HASH_LIST_H
#define HASH_LIST_H

#include <list>
#include "maskedptr.h"
#include "ireferent.h"
#include "iref.h"
#include "ref.h"

namespace sakusen {

template<typename T>
class hash_list_iterator;

template<typename T>
class hash_list_const_iterator;

template<typename T>
class hash_list : private IRefContainer, boost::noncopyable {
  friend class hash_list_iterator<T>;
  friend class hash_list_const_iterator<T>;
  public:
    typedef hash_list_iterator<T> iterator;
    typedef hash_list_const_iterator<T> const_iterator;
    typedef size_t size_type;
    
    hash_list();
    ~hash_list();
  private:
    typedef std::list<T*> List;
    typedef __gnu_cxx::hash_map<MaskedPtr<T>, typename List::iterator>
      ListIteratorHash;
    typedef __gnu_cxx::hash_multimap<MaskedPtr<IReferent>, IRef*> RefHash;
    
    List list;
    /** \brief A hash_map of iterators from the above list
     *
     * This allow a unit from the above list to
     * be erased in O(1) time from only its pointer.  It may seem perverse to
     * have the list at all, but that is important to guarantee iteration
     * ordering - different implementations of hash_map might iterate their
     * members in different orders, thus yielding non-reproducibility */
    ListIteratorHash listIts;
    mutable RefHash refs;
    
    void invalidateRefs(const MaskedPtr<IReferent>& id);
    void registerRef(IRef*) const;
    void unregisterRef(IRef*) const;
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
    iterator push_back(T* item);
    void pop_front();
    void pop_back();
    void insert(const_iterator pos, T* item);
    template<typename InputIterator>
    void insert(const_iterator pos, InputIterator first, InputIterator last);
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
void hash_list<T>::invalidateRefs(const MaskedPtr<IReferent>& id)
{
  std::pair<typename RefHash::iterator, typename RefHash::iterator> refRange =
    refs.equal_range(id);
  
  for (typename RefHash::iterator refIt = refRange.first;
      refIt != refRange.second; ++refIt) {
    refIt->second->invalidate();
  }
  refs.erase(refRange.first, refRange.second);
}

template<typename T>
void hash_list<T>::registerRef(IRef* ref) const
{
  MaskedPtr<IReferent> id(*ref);
  std::pair<MaskedPtr<IReferent>, IRef*> item(id, ref);
  refs.insert(item);
}

template<typename T>
void hash_list<T>::unregisterRef(IRef* ref) const
{
  std::pair<typename RefHash::iterator, typename RefHash::iterator> refRange =
    refs.equal_range(*ref);
  
  /* This becomes slow if there are many Refs to the same object.
   * If that is a problem we could ID the refs too and have O(1) lookups for
   * them, but it's unlikely to be of help overall */
  for (typename RefHash::iterator refIt = refRange.first;
      refIt != refRange.second; ++refIt) {
    if (refIt->second == ref) {
      refs.erase(refIt);
      return;
    }
  }
  Fatal("tried to unregister a not-registered Ref");
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
inline Ref<T> hash_list<T>::back()
{
  return Ref<T>(list.back(), this);
}

template<typename T>
typename hash_list<T>::iterator hash_list<T>::push_back(T* item)
{
  item->supplyRef(Ref<T>(item, this));
  list.push_back(item);
  typename List::iterator it = list.end();
  --it;

  std::pair<typename ListIteratorHash::iterator, bool> result =
    listIts.insert(std::pair<MaskedPtr<T>, typename List::iterator>(item, it));

  if (!result.second) {
    throw std::logic_error("duplicate pointer inserted");
  }

  return iterator(it, this);
}

template<typename T>
typename hash_list<T>::iterator hash_list<T>::erase(iterator pos)
{
  iterator next = pos;
  ++next;
  T* ptr = *pos.listIt;
  invalidateRefs(ptr);
  listIts.erase(ptr);
  delete ptr;
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
typename hash_list<T>::iterator hash_list<T>::erase(
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
  assert(refs.empty());
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
inline bool operator!=(
    const hash_list_const_iterator<T>& left,
    const hash_list_const_iterator<T>& right
  )
{
  return (left.listIt != right.listIt);
}

template<typename T>
inline bool hash_list_iterator<T>::operator==(const hash_list_iterator& right)
{
  return (listIt == right.listIt);
}

template<typename T>
inline bool hash_list_iterator<T>::operator!=(const hash_list_iterator& right)
{
  return (listIt != right.listIt);
}

template<typename T>
inline Ref<T> hash_list_iterator<T>::operator*() const
{
  return Ref<T>(*listIt, container);
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
inline Ref<const T> hash_list_const_iterator<T>::operator*() const
{
  return Ref<const T>(*listIt, container);
}

template<typename T>
inline hash_list_const_iterator<T>& hash_list_const_iterator<T>::operator++()
{
  ++listIt;
  return *this;
}

}

#endif // HASH_LIST_H

