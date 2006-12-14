#ifndef LIBSAKUSEN__REF_H
#define LIBSAKUSEN__REF_H

#include "iarchive.h"
#include "oarchive.h"
#include "maskedptr.h"
#include "typemunger.h"
#include "irefcontainer.h"

namespace sakusen {

template<typename T>
class SerializationHandler;

template<typename T>
class hash_list;

/** \brief A reference to a game object managed by the world.
 *
 * Rather than simply storing a pointer to a game object which might be left
 * dangling after that object is removed, code can request from world a
 * reference to an object of this form.  This reference will automatically keep
 * track of the objects removal, and invalidate itself when that happens */
template<class T>
class Ref : public IRef {
  friend class hash_list<T>;
  private:
    typedef SerializationHandler<typename TypeMunger<T>::unconsted> SHandler;
  public:
    Ref() : container(NULL), referee(NULL) {}
    Ref(T* r, const IRefContainer* c) : container(c), referee(r)
    {
      assert(referee != NULL);
      container->registerRef(this);
    }
    Ref(const Ref<T>& copy) :
      IRef(copy),
      container(copy.container),
      referee(copy.referee)
    {
      if (isValid()) {
        container->registerRef(this);
      }
    }
    Ref<T>& operator=(const Ref<T>& copy) {
      if (isValid()) {
        container->unregisterRef(this);
      }
      container = copy.container;
      referee = copy.referee;
      if (isValid()) {
        container->registerRef(this);
      }
      return *this;
    }
    ~Ref() {
      if (isValid()) {
        container->unregisterRef(this);
      }
      container = NULL;
      referee = NULL;
    }
    operator MaskedPtr<typename TypeMunger<T>::unconsted>() const {
      return MaskedPtr<typename TypeMunger<T>::unconsted>(referee);
    }
    template<typename U>
    operator Ref<U>() const {
      return Ref<U>(referee, container);
    }
  private:
    const IRefContainer* container;
    T* referee;

    inline void invalidate() {
      container = NULL;
      referee = NULL;
    }

    operator MaskedPtr<IReferent>() const {
      return MaskedPtr<IReferent>(referee);
    }

    IRef* newCopy() const {
      return new Ref<T>(*this);
    }
  public:
    inline bool isValid() const { return referee != NULL; }
    
    inline bool isRefTo(const T* t) const { return referee == t; }
    
    inline T* operator->() const {
      assert(isValid());
      return referee;
    }

    template<typename U>
    inline Ref<U> cast() const {
      return Ref<U>(referee, container);
    }

    void store(OArchive& archive) const {
      archive << isValid();
      if (isValid()) {
        SHandler().insert(archive, cast<typename TypeMunger<T>::consted>());
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

}

#endif // LIBSAKUSEN__REF_H

