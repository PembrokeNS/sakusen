#ifndef OARCHIVE_H
#define OARCHIVE_H

#include "libsakusen-global.h"

#include <boost/multi_array.hpp>

#include "stringutils.h"
#include "point.h"

namespace sakusen {

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
    inline void store(const T& toStore);
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
    OArchive& operator<<(const uint8& i) {
      ensureSpace(sizeof(uint8));
      buffer[length]=i;
      length+=sizeof(uint8);
      return *this;
    }
    OArchive& operator<<(const sint8& i) {
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

    template<typename T>
    OArchive& insertEnum(const T& value)
    {
      return *this << static_cast<uint8>(value);
    }
    
    template<typename T, size_t size>
    OArchive& insert(const T toStore[size])
    {
      for (size_t i=0; i<size; ++i) {
        store<T>(toStore[i]);
      }

      return *this;
    }
    
    template<typename T>
    OArchive& insert(const T* toStore, size_t size)
    {
      for (size_t i=0; i<size; ++i) {
        store<T>(toStore[i]);
      }

      return *this;
    }
    
    template<typename T, size_t size>
    OArchive& insert(const boost::array<T, size>& toStore)
    {
      for (size_t i=0; i<size; ++i) {
        store<T>(toStore[i]);
      }

      return *this;
    }

    template<typename T, size_t rank>
	  OArchive& insert(const boost::multi_array<T, rank>& toStore)
    {
      /** \bug Handles only zero-based-indexed multi_arrays */
      assert(rank == toStore.num_dimensions());
      boost::array<uint32, rank> shape;
      std::copy(toStore.shape(), toStore.shape()+rank, shape.begin());
      insert<uint32, rank>(shape);

      boost::array<uint32, rank> i;
      std::fill(i.begin(), i.end(), 0);
      uint32 j;

      do {
        store(toStore(i));
        for (j=0; j<rank; ++j) {
          if (i[j] == shape[j]) {
            i[j] = 0;
          } else {
            ++i[j];
            break;
          }
        }
      } while (j < rank);

      return *this;
    }

    template<typename T>
	  OArchive& operator<<(const std::vector<T>& toStore)
    {
      *this << uint32(toStore.size());
  
      for(typename std::vector<T>::const_iterator i = toStore.begin();
          i != toStore.end(); ++i) {
        store(*i);
      }

      return *this;
    }

    template<typename T>
	  OArchive& operator<<(const std::vector<std::vector<T> >& toStore)
    {
      *this << uint32(toStore.size());
  
      for(typename std::vector<std::vector<T> >::const_iterator i =
          toStore.begin(); i != toStore.end(); ++i) {
        *this << *i;
      }

      return *this;
    }

    template<typename T>
    OArchive& operator<<(const std::list<T>& toStore)
    {
      *this << uint32(toStore.size());
  
      for(typename std::list<T>::const_iterator i = toStore.begin();
          i != toStore.end(); ++i) {
        store(*i);
      }

      return *this;
    }

    template<typename T, typename U, typename THash>
    OArchive& operator<<(const __gnu_cxx::hash_map<T, U, THash>& toStore)
    {
      *this << uint32(toStore.size());
  
      for(typename __gnu_cxx::hash_map<T, U, THash>::const_iterator i =
          toStore.begin(); i != toStore.end(); ++i) {
        store(i->first);
        store(i->second);
      }

      return *this;
    }
    
    template<typename T>
    inline OArchive& operator<<(const Point<T>& p)
    {
      return *this << p.x << p.y << p.z;
    }

    template<int size>
    inline void magicValue(const char val[size+1]) {
      /* The '+1' in the array length is to allow for the trailing null char */
      ensureSpace(size);
      memcpy(buffer+length, val, size);
      length += size;
    }
};

}

#endif // OARCHIVE_H

