#ifndef MASKEDPTR_H
#define MASKEDPTR_H

#include <boost/shared_ptr.hpp>

#include "gnu_extensions.h"
#include "getptr.h"

namespace sakusen {

/** \brief Hides a pointer so that it cannot be dereferenced, but can still be
 * used as a UID.
 *
 * Beware, because although addresses serve as good UIDs for things that exist,
 * they can be swiftly reused after an object is deleted.
 */
template<typename T>
class MaskedPtr {
  friend class __gnu_cxx::hash<MaskedPtr<T> >;
  public:
    MaskedPtr(const T* p) : ptr(p) {}
    MaskedPtr(const boost::shared_ptr<T>& p) : ptr(p.get()) {}
    MaskedPtr(const boost::variant<T*, boost::shared_ptr<T> >& p)
    {
      GetPtr<T> gp;
      ptr = p.apply_visitor(gp);
    }
    MaskedPtr(const boost::shared_ptr<const T>& p) : ptr(p.get()) {}
    template<typename U>
    explicit MaskedPtr(MaskedPtr<U> p) : ptr(p.ptr) {}
    bool operator==(const MaskedPtr& right) const { return ptr == right.ptr; }
  private:
    const T* ptr;
  public:
};

}

namespace __gnu_cxx {

template<typename T>
class hash<sakusen::MaskedPtr<T> > {
  public:
    inline size_t operator()(const sakusen::MaskedPtr<T>& p) const {
      return reinterpret_cast<size_t>(p.ptr);
    }
};

}

#endif // MASKEDPTR_H

