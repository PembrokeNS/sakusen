#ifndef OARCHIVE_H
#define OARCHIVE_H

#include "libsakusen-global.h"

#include <boost/multi_array.hpp>
#include <boost/call_traits.hpp>

#include "gnu_extensions.h"
#include "stringutils.h"
#include "point.h"
#include "idbase.h"

namespace sakusen {

class IArchive;
class OArchive;

/** \internal \brief Helper class to handle storing of different types in an
 * OArchive */
template<typename T>
struct Storer {
  void operator()(OArchive&, typename boost::call_traits<T>::param_type item);
};

/** \brief This class represents an archive into which we put objects for
 * tranmission over the network or saving to disk.
 *
 * \see IArchive
 */
class LIBSAKUSEN_API OArchive {
  public:
    OArchive();
    OArchive(size_t startCapacity);
    OArchive(const OArchive& copy);
    ~OArchive();
    OArchive& operator=(const OArchive& copy);
    OArchive& operator=(const IArchive& copy);
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
  public:
    inline size_t getLength() const { return length; }
    /** \brief Get a pointer to the OArchive's buffer
     *
     * This can be used, for example, to transmit the archive over a network.
     *
     * \warning This is a pointer to memory owned by the OArchive.  Don't mess
     * with it.
     */
    inline const uint8* getBytes() const { return buffer; }
    inline size_t getBytesLength() const { return length; }
    /** \brief Get an ASCII hex version of a hash of the contents of the
     * OArchive at present.
     *
     * This hash will change as things are inserted into the OArchive. */
    inline String getSecureHashAsString() const {
      return stringUtils_getSecureHashAsString(buffer, length);
    }
    void dumpBuffer() const;
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
    OArchive& operator<<(const uint64& i);
    OArchive& operator<<(const sint64& i);
    OArchive& operator<<(const double& d);
    OArchive& operator<<(const String& s);
    OArchive& operator<<(const OArchive&);

    /** \brief Insert an enum value into the OArchive
     *
     * It is important to use this method to insert enum values rather than
     * using operator<< because the underlying integer type of an enum is
     * platform-dependent.
     *
     * \warning This assumes that the enum values all fall in the range 0-255
     */
    template<typename T>
    OArchive& insertEnum(const T value) {
      return *this << static_cast<uint8>(value);
    }

    template<typename T, typename U>
    OArchive& operator<<(const std::pair<T, U>& toStore) {
      Storer<T>()(*this, toStore.first);
      Storer<U>()(*this, toStore.second);
      return *this;
    }

    template<typename TInteger, typename TBase>
    OArchive& operator<<(const IdBase<TInteger, TBase>& toStore) {
      return *this << toStore.val;
    }
    
    template<typename T, size_t size>
    OArchive& operator<<(const T (&toStore)[size]) {
      Storer<T> storer;
      for (size_t i=0; i<size; ++i) {
        storer(*this, toStore[i]);
      }

      return *this;
    }
    
    template<typename T>
    OArchive& insert(const T* toStore, size_t size) {
      Storer<T> storer;
      for (size_t i=0; i<size; ++i) {
        storer(*this, toStore[i]);
      }

      return *this;
    }
    
    template<typename T, size_t size>
    OArchive& insert(const boost::array<T, size>& toStore) {
      Storer<T> storer;
      for (size_t i=0; i<size; ++i) {
        storer(*this, toStore[i]);
      }

      return *this;
    }

    template<typename T, size_t rank>
	  OArchive& insert(const boost::multi_array<T, rank>& toStore) {
      /** \bug Handles only zero-based-indexed multi_arrays */
      assert(rank == toStore.num_dimensions());
      boost::array<uint32, rank> shape;
      std::copy(toStore.shape(), toStore.shape()+rank, shape.begin());
      insert<uint32, rank>(shape);

      /* If any dimension is zero, there are no entries, and we quit now */
      if (shape.end() != find(shape.begin(), shape.end(), 0U)) {
        return *this;
      }

      boost::array<uint32, rank> i;
      std::fill(i.begin(), i.end(), 0);
      uint32 j;
      Storer<T> storer;

      do {
        storer(*this, toStore(i));
        for (j=0; j<rank; ++j) {
          if (++(i[j]) == shape[j]) {
            i[j] = 0;
          } else {
            break;
          }
        }
      } while (j < rank);

      return *this;
    }

    template<typename T>
	  OArchive& operator<<(const std::vector<T>& toStore) {
      *this << uint32(toStore.size());
      Storer<T> storer;
  
      for(typename std::vector<T>::const_iterator i = toStore.begin();
          i != toStore.end(); ++i) {
        storer(*this, *i);
      }

      return *this;
    }

    template<typename T>
    OArchive& operator<<(const std::list<T>& toStore) {
      *this << uint32(toStore.size());
      Storer<T> storer;
  
      for(typename std::list<T>::const_iterator i = toStore.begin();
          i != toStore.end(); ++i) {
        storer(*this, *i);
      }

      return *this;
    }

    template<typename T, typename U, typename THash>
    OArchive& operator<<(const __gnu_cxx::hash_map<T, U, THash>& toStore) {
      *this << uint32(toStore.size());
      Storer<std::pair<T, U> > storer;
  
      for(typename __gnu_cxx::hash_map<T, U, THash>::const_iterator i =
          toStore.begin(); i != toStore.end(); ++i) {
        storer(*this, *i);
      }

      return *this;
    }
    
    template<typename T>
    inline OArchive& operator<<(const Point<T>& p) {
      return *this << p.x << p.y << p.z;
    }

    inline OArchive& magicValue(const String& val) {
      ensureSpace(val.size());
      memcpy(buffer+length, val.c_str(), val.size());
      length += val.size();
      return *this;
    }
};

}

#endif // OARCHIVE_H

