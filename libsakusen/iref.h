#ifndef IREF_H
#define IREF_H

#include "maskedptr.h"

namespace sakusen {

/** \brief Interface for Ref<T>s for use in circumstances when you don't know
 * what T is
 *
 * If a container is managing objects which are referred to by Refs then it
 * needs to maintain a list of those Refs.  However, even if the container is a
 * container of objects of type T, then not all the Refs to those objects need
 * be Ref<T>s.  Some could be Ref<U>s where U is some other class in the class
 * heirarchy of T (either above or below T).  So, this container of Refs can't
 * be a container of Ref<T>s alone, it has to be a container of all sorts of
 * Refs.  To allow this, Ref<T> inherits from IRef, which is not templated.
 *
 * IRef could also be used in any other circumstance where you want to handle a
 * mixture of Refs to different types, or even just a single Ref without
 * knowing what type it refers to.  However, if you're using but a single type,
 * then you should probably be using Ref<T>s.
 */
class IRef {
  protected:
    IRef() {}
    IRef(const IRef&) {}
  public:
    virtual ~IRef() {}
    virtual IRef* newCopy() const = 0;
};

}

#endif // IREF_H

