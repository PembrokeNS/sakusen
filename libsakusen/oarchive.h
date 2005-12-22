#ifndef OARCHIVE_H
#define OARCHIVE_H

#include "libsakusen-global.h"

#include "stringutils.h"
#include "point.h"

/* This class represents an archive into which we put objects for tranmission
 * over the network or saving to disk. */
class LIBSAKUSEN_API OArchive {
  public:
    OArchive();
    OArchive(size_t startCapacity);
    OArchive(const OArchive& copy);
    ~OArchive();
    OArchive& operator=(const OArchive& copy);
  private:
    uint8* buffer; /* owned by this */
    size_t length;
    size_t capacity;
    void expand();
    inline void ensureSpace(size_t space) {
      while(capacity-length < space) {
        expand();
      }
    }

    template<typename T>
    inline void store(const T&);
  public:
    inline size_t getLength() const { return length; }
    inline const uint8* getBytes() const { return buffer; }
      /* Danger: we're revealing a pointer to memory we own!  Possibly we
       * should copy it instead. */
    inline const size_t getBytesLength() const { return length; }
    inline String getSecureHashAsString() const {
      return stringUtils_getSecureHashAsString(buffer, length);
    }
    inline OArchive& operator<<(const bool& i) {
      ensureSpace(sizeof(uint8));
      buffer[length] = i;
      length += sizeof(uint8);
      return *this;
    }
    inline OArchive& operator<<(const uint8& i) {
      ensureSpace(sizeof(uint8));
      buffer[length]=i;
      length+=sizeof(uint8);
      return *this;
    }
    inline OArchive& operator<<(const sint8& i) {
      ensureSpace(sizeof(sint8));
      buffer[length] = i;
      length+=sizeof(sint8);
      return *this;
    }
    OArchive& operator<<(const uint16& i);
    OArchive& operator<<(const sint16& i);
    OArchive& operator<<(const uint32& i);
    OArchive& operator<<(const sint32& i);
    OArchive& operator<<(const double& d);
    OArchive& operator<<(const String& s);

    template<typename T, template<typename> class Container>
    OArchive& operator<<(const Container<T>&);
    
    template<
        typename T,
        template<typename> class Container1,
        template<typename> class Container2
      >
    OArchive& operator<<(const Container1< Container2<T> >& toStore);

    template<typename T>
    inline OArchive& operator<<(const Point<T>&);
};

#endif // OARCHIVE_H

