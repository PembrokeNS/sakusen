#ifndef LIBSAKUSEN__OARCHIVE_H
#define LIBSAKUSEN__OARCHIVE_H

#include <sakusen/global.h>

#include <set>

#include <boost/multi_array.hpp>
#include <boost/call_traits.hpp>

#include <sakusen/gnu_extensions.h>
#include <sakusen/stringutils.h>
#include <sakusen/point.h>
#include <sakusen/idbase.h>
#include <sakusen/buffer.h>

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
class LIBSAKUSEN_API OArchive : private boost::noncopyable {
  public:
    OArchive();
    OArchive(size_t startCapacity);
  private:
    Buffer buffer;
  public:
    /** \brief Get the OArchive's buffer
     *
     * This can be used, for example, to transmit the archive over a network.
     */
    inline const Buffer& getBuffer() const { return buffer; }
    /** \brief Get an ASCII hex version of a hash of the contents of the
     * OArchive at present.
     *
     * This hash will change as things are inserted into the OArchive. */
    inline String getSecureHashAsString() const {
      return buffer.getSecureHashAsString();
    }
    void dumpBuffer() const;
    inline OArchive& operator<<(const bool& i) {
      buffer.appendByte(i);
      return *this;
    }
    OArchive& operator<<(const uint8& i) {
      buffer.appendByte(i);
      return *this;
    }
    OArchive& operator<<(const sint8& i) {
      buffer.appendByte(i);
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
    OArchive& operator<<(const Buffer&);

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
      if (shape.end() != std::find(shape.begin(), shape.end(), 0U)) {
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

    template<typename T>
    OArchive& operator<<(const std::set<T>& toStore) {
      *this << uint32(toStore.size());
      Storer<T> storer;
  
      for(typename std::set<T>::const_iterator i = toStore.begin();
          i != toStore.end(); ++i) {
        storer(*this, *i);
      }

      return *this;
    }

    /* Can't use the u_map metafunction because that prevents type inference */
    template<typename T, typename U, typename THash>
#ifdef SAKUSEN_USE_UNORDERED_MAP
    OArchive& operator<<(
        const SAKUSEN_UNORDERED_MAP_NAMESPACE::unordered_map<T, U, THash>&
          toStore
      )
#else
    OArchive& operator<<(const __gnu_cxx::hash_map<T, U, THash>& toStore)
#endif
    {
      *this << uint32(toStore.size());
      Storer<std::pair<T, U> > storer;
  
      for(typename u_map<T, U, THash>::type::const_iterator i =
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
      buffer.appendBytes(val.c_str(), val.size());
      return *this;
    }
};

}

#endif // LIBSAKUSEN__OARCHIVE_H

