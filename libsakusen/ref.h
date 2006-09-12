#ifndef LIBSAKUSEN__REF_H
#define LIBSAKUSEN__REF_H

#include "iarchive.h"
#include "oarchive.h"

namespace sakusen {

template<typename T>
class RefHandler;

/** \brief A reference to a game object managed by the world.
 *
 * Rather than simply storing a pointer to a game object which might be left
 * dangling after that object is removed, code can request from world a
 * reference to an object of this form.  This reference will automatically keep
 * track of the objects removal, and invalidate itself when that happens */
template<typename T>
class Ref {
  public:
    Ref() : referee(NULL) {}
    Ref(const Ref<T>& copy) : referee(copy.referee) {
      if (isValid()) {
        handler.registerRef(this);
      }
    }
    Ref<T>& operator=(const Ref<T>& copy) {
      if (isValid()) {
        handler.unregisterRef(this);
      }
      referee = copy.referee;
      if (isValid()) {
        handler.registerRef(this);
      }
      return *this;
    }
    Ref(T* r) : referee(r)
    {
      handler.registerRef(this);
    }
    ~Ref() {
      if (isValid()) {
        handler.unregisterRef(this);
      }
    }
  private:
    RefHandler<T> handler;
    T* referee;
  public:
    inline bool isValid() const { return referee != NULL; }
    
    inline T* operator->() {
      assert(isValid());
      return referee;
    }
    
    inline const T* operator->() const {
      assert(isValid());
      return referee;
    }

    inline T& operator*() {
      assert(isValid());
      return *referee;
    }

    inline const T& operator*() const {
      assert(isValid());
      return *referee;
    }

    inline void invalidate() {
      referee = NULL;
    }

    void store(OArchive& archive) const {
      archive << isValid();
      if (isValid()) {
        handler.insert(archive, referee);
      }
    }
    
    static Ref<T> load(
        IArchive& archive, const typename RefHandler<T>::loadArgument* arg
      ) {
      bool valid;
      T* referee = NULL;
      
      archive >> valid;
      if (!valid) {
        return Ref<T>();
      }
      
      referee = RefHandler<T>().extract(archive, arg);
      /* Even though the reference was valid when serialized, the thing it
       * refers to might have vanished */
      if (NULL == referee) {
        return Ref<T>();
      }

      return Ref<T>(referee);
    }
};

}

#endif // LIBSAKUSEN__REF_H

