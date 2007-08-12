#ifndef MASKEDPTR_H
#define MASKEDPTR_H

#include <boost/shared_ptr.hpp>

#include "gnu_extensions.h"
#include "getptr.h"

namespace sakusen {

template<typename T>
class MaskedPtr;

template<typename T>
bool operator==(const MaskedPtr<T>& left, const MaskedPtr<T>& right);

/** \brief Hides a pointer so that it cannot be dereferenced, but can still be
 * used as a UID.
 *
 * Beware, because although addresses serve as good UIDs for things that exist,
 * they can be swiftly reused after an object is deleted.
 */
template<typename T>
class MaskedPtr {
  friend bool operator==<T>(const MaskedPtr& left, const MaskedPtr& right);
#ifdef _MSC_VER //STLport's hash is a struct, not a class. 
  friend struct std::hash < MaskedPtr<T> >;
#else
  friend class __gnu_cxx::hash<MaskedPtr<T> >;
#endif //_MSC_VER
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
  private:
    const T* ptr;
  public:
};

template<typename T>
inline bool operator==(const MaskedPtr<T>& left, const MaskedPtr<T>& right) {
  return left.ptr == right.ptr;
}

}

namespace __gnu_cxx {
#ifdef _MSC_VER //STLport's hash is a struct, not a class. 
	template<typename T>
	struct hash<sakusen::MaskedPtr<T> > {
#else
	template<typename T>
	class hash<sakusen::MaskedPtr<T> > {
#endif //_MSC_VER
	  public:
		inline size_t operator()(const sakusen::MaskedPtr<T>& p) const {
		  return reinterpret_cast<size_t>(p.ptr);
		}
	};

}

#endif // MASKEDPTR_H

