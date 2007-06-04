#ifndef LIBSAKUSEN__REF_H
#define LIBSAKUSEN__REF_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "iarchive.h"
#include "oarchive.h"
#include "maskedptr.h"

namespace sakusen {

template<typename T>
class SerializationHandler;

template<typename T>
class Ref;

template<typename T>
bool operator==(const Ref<T>& left, const Ref<T>& right);

/** \brief A reference to a game object managed by the world.
 *
 * Rather than simply storing a pointer to a game object which might be left
 * dangling after that object is removed, code can request from world a
 * reference to an object of this form.  This reference will automatically keep
 * track of the objects removal, and invalidate itself when that happens */
template<class T>
class Ref {
  friend bool operator==<T>(const Ref<T>&, const Ref<T>&);
  private:
    typedef
      SerializationHandler<typename boost::remove_const<T>::type> SHandler;
    typedef MaskedPtr<typename boost::remove_const<T>::type> MPtr;
  public:
    Ref() : referee() {}
    Ref(boost::weak_ptr<T> r) : referee(r) {}
    operator MPtr() const {
      return MPtr(referee.lock());
    }
    template<typename U>
    operator Ref<U>() const {
      return Ref<U>(referee);
    }
  private:
    boost::weak_ptr<T> referee;
  public:
    inline bool isValid() const { return !referee.expired(); }
    
    inline bool isRefTo(const T* t) const { return referee.lock().get() == t; }
    
    inline boost::shared_ptr<T> operator->() const {
      return referee.lock();
    }

    template<typename U>
    inline Ref<U> cast() const {
      return Ref<U>(referee);
    }

    /* Note that the names dynamic_cast, static_cast cannot be used for methods
     * */
    template<typename U>
    inline Ref<U> dynamicCast() const {
      return Ref<U>(boost::dynamic_pointer_cast<U>(referee.lock()));
    }

    template<typename U>
    inline Ref<U> staticCast() const {
      return Ref<U>(boost::static_pointer_cast<U>(referee.lock()));
    }

    void store(OArchive& archive) const {
      archive << isValid();
      if (isValid()) {
        SHandler().insert(archive, cast<typename boost::add_const<T>::type>());
      }
    }
    
    static Ref<T> load(
        IArchive& archive,
        const typename SHandler::loadArgument* arg
      ) {
      bool valid;
      archive >> valid;
      if (!valid) {
        return Ref<T>();
      }
      
      return SHandler().extract(archive, arg);
    }
};

template<typename T>
inline bool operator==(const Ref<T>& left, const Ref<T>& right)
{
  return left.isValid() && right.isValid() &&
    left.referee.lock() == right.referee.lock();
}

}

#endif // LIBSAKUSEN__REF_H

