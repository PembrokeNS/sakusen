#ifndef LIBSAKUSEN__GETPTR_H
#define LIBSAKUSEN__GETPTR_H

#include <boost/variant.hpp>

namespace sakusen {

/** \brief Function object for extracting pointer in a generic fashion */
template<typename T>
struct GetPtr : public boost::static_visitor<T*> {
  template<typename P>
  T* operator()(const P& p) const { return /*boost::*/get_pointer(p); }

  T* operator()(T* p) const { return p; }
};

}

#endif // LIBSAKUSEN__GETPTR_H

