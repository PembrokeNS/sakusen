#ifndef IARCHIVE_H
#define IARCHIVE_H

#include "libsakusen-global.h"

#include <vector>
#include <list>
#include <boost/shared_array.hpp>
#include <boost/multi_array.hpp>
#include <boost/utility.hpp>

#include "gnu_extensions.h"
#include "point.h"
#include "exceptions.h"
#include "stringutils.h"
#include "resourceinterface.h"

namespace sakusen {

class OArchive;

/** \brief Class used inside IArchive for working with boost::multi_array */  
template<size_t pos>
class extent_generator {
  public:
    template<typename Collection>
    inline boost::detail::multi_array::extent_gen<pos> operator()(
        const Collection& c
      ) const {
      return extent_generator<pos-1>()(c)[c[pos-1]];
    }
};

template<>
class extent_generator<0> {
  public:
    template<typename Collection>
    inline boost::detail::multi_array::extent_gen<0> operator()(
        const Collection&
      ) const {
      return boost::extents;
    }
};

/** \brief Source from which objects can be read.
 *
 * This class represents an archive from which we extract objects after
 * receiving it across the network or reading it from disk.  c.f. OArchive.
 * Thus it can be constructed from a buffer of uint8s and has lots of overloads
 * of operator>>.
 *
 * It's worth noting that the libstdc++ docs say that doing precisely this is a
 * Bad Thing, because the << and >> operators should only be used for formatted
 * input and output.  The correct solution is make these classes ([IO]Archive)
 * subclasses of streambuf, but I don't know how to do that.
 * Besides that, we have the problem of differnt floating point formats (solved
 * at present by storing such numbers as text).
 *
 * \see OArchive
 * \todo deal with all of the above
 *
 * \note The name IArchive follows the istream/ostream convention.
 */
class LIBSAKUSEN_API IArchive {
  friend class OArchive;
  public:
    IArchive();
    IArchive(const IArchive&);
    IArchive(const uint8* buffer, size_t length);
    IArchive(const boost::shared_array<uint8>& buffer, size_t length);
    IArchive(const boost::shared_array<const uint8>& buffer, size_t length);
    IArchive(const OArchive&);
    IArchive& operator=(const IArchive&);
    ~IArchive();
  private:
    uint8* originalBuffer; /* owned by this */
    const uint8* buffer;
    size_t remainingLength;
    inline void assertLength(size_t length) const {
      /* We ensure that enough length remains.
       * If not, we throw an exception */
      if (remainingLength < length) {
        throw EndOfArchiveDeserializationExn();
      }
    }
    inline void advance(size_t length) {
      buffer += length;
      remainingLength -= length;
    }

    template<typename T>
    inline T load();

    template<typename T>
    inline T load(const typename T::loadArgument* arg)
    {
      return T::load(*this, arg);
    }

    template<typename T>
    inline T load(
        const typename T::loadArgument* arg,
        const typename T::loadArgument2* arg2
      )
    {
      return T::load(*this, arg, arg2);
    }

    template<typename T>
    inline T load(ResourceInterface::Ptr resourceInterface)
    {
      return T::load(*this, resourceInterface);
    }
  public:
    inline String getSecureHashAsString() const {
      return stringUtils_getSecureHashAsString(
          originalBuffer, (buffer+remainingLength)-originalBuffer
        );
    }
    inline bool isFinished() const { return remainingLength == 0; }
    /* \brief Dump the buffer as hex to stdout
     *
     * \note For debugging purposes */
    void dumpBuffer() const;
    inline IArchive& operator>>(bool& i) {
      assertLength(sizeof(uint8));
      assert(*buffer <= 1);
      i = !(0 == *buffer);
      advance(sizeof(uint8));
      return *this;
    }
    inline IArchive& operator>>(uint8& i) {
      assertLength(sizeof(uint8));
      i = *buffer;
      advance(sizeof(uint8));
      return *this;
    }
    inline IArchive& operator>>(sint8& i) {
      assertLength(sizeof(sint8));
      i = *buffer;
      advance(sizeof(sint8));
      return *this;
    }
    IArchive& operator>>(uint16& i);
    IArchive& operator>>(sint16& i);
    IArchive& operator>>(uint32& i);
    IArchive& operator>>(sint32& i);
    IArchive& operator>>(double& d);
    IArchive& operator>>(String& s);
    IArchive& operator>>(IArchive&);

    template<typename T>
    IArchive& extractEnum(T& result)
    {
      uint8 valAsInt;
      *this >> valAsInt;
      result = static_cast<T>(valAsInt);
      return *this;
    }
    
    template<typename T, size_t size>
    IArchive& extract(T result[size])
    {
      for (int i=0; i<size; ++i) {
        result[i]=load<T>();
      }

      return *this;
    }
    
    template<typename T, size_t size>
    IArchive& extract(T result[size], const typename T::loadArgument* arg)
    {
      for (size_t i=0; i<size; ++i) {
        result[i]=load<T>(arg);
      }

      return *this;
    }
    
    template<typename T>
    IArchive& extract(T* result, size_t size)
    {
      for (size_t i=0; i<size; ++i) {
        result[i] = load<T>();
      }

      return *this;
    }
    
    template<typename T, size_t size>
    IArchive& extract(boost::array<T, size>& result)
    {
      for (size_t i=0; i<size; ++i) {
        result[i]=load<T>();
      }

      return *this;
    }

  public:

    template<typename T, size_t rank>
	  IArchive& extract(boost::multi_array<T, rank>& result)
    {
      assert(rank == result.num_dimensions());
      boost::array<uint32, rank> shape;
      extract<uint32, rank>(shape);
      
      result.resize(extent_generator<rank>()(shape));

      // If any dimension is zero, there are no entries, and we quit now
      if (shape.end() != find(shape.begin(), shape.end(), 0U)) {
        return *this;
      }

      boost::array<uint32, rank> i;
      std::fill(i.begin(), i.end(), 0);
      uint32 j;

      do {
        result(i) = load<T>();
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
    IArchive& operator>>(std::vector<T>& result)
    {
      assert(result.empty());
      uint32 size;
      *this >> size;

      while (size--) {
        result.push_back(load<T>());
      }

      return *this;
    }
    
    template<typename T>
    IArchive& operator>>(std::vector<std::vector<T> >& result)
    {
      assert(result.empty());
      uint32 size;
      *this >> size;

      while (size--) {
        std::vector<T> tmp;
        *this >> tmp;
        result.push_back(tmp);
      }

      return *this;
    }
    
    template<typename T>
    IArchive& extract(
        std::vector<T>& result,
        ResourceInterface::Ptr resourceInterface
      )
    {
      assert(result.empty());
      uint32 size;
      *this >> size;

      while (size--) {
        result.push_back(load<T>(resourceInterface));
      }

      return *this;
    }

    template<typename T>
    IArchive& operator>>(std::list<T>& result)
    {
      assert(result.empty());
      uint32 size;
      *this >> size;

      while (size--) {
        result.push_back(load<T>());
      }

      return *this;
    }

    template<typename T, typename U, typename THash>
    IArchive& operator>>(__gnu_cxx::hash_map<T, U, THash>& result)
    {
      assert(result.empty());
      uint32 size;
      *this >> size;

      while (size--) {
        if (!result.insert(std::pair<T, U>(load<T>(), load<U>())).second) {
          throw DeserializationExn("Duplicate key in hash_map");
        }
      }

      return *this;
    }

    template<typename T>
    IArchive& extract(
        std::vector<T>& result,
        const typename T::loadArgument* arg
      )
    {
      assert(result.empty());
      uint32 size;
      *this >> size;

      while (size--) {
        result.push_back(load<T>(arg));
      }

      return *this;
    }

    template<typename T>
    IArchive& extract(
        std::list<T>& result,
        const typename T::loadArgument* arg
      )
    {
      assert(result.empty());
      uint32 size;
      *this >> size;

      while (size--) {
        result.push_back(load<T>(arg));
      }

      return *this;
    }

    template<typename T>
    IArchive& extract(
        std::list<T>& result,
        const typename T::loadArgument* arg,
        const typename T::loadArgument2* arg2
      )
    {
      assert(result.empty());
      uint32 size;
      *this >> size;

      while (size--) {
        result.push_back(load<T>(arg, arg2));
      }

      return *this;
    }

    template<typename T>
    IArchive& operator>>(Point<T>& result)
    {
      T x;
      *this >> x;
      T y;
      *this >> y;
      T z;
      *this >> z;

      result = Point<T>(x,y,z);
      return *this;
    }

    inline void magicValue(const String& val) {
      assertLength(val.size());
      if (0 != memcmp(val.c_str(), buffer, val.size())) {
        Debug("Wrong magic");
        throw WrongMagicDeserializationExn(val, buffer);
      }
      advance(val.size());
    }
};

template<>
inline uint8 IArchive::load<uint8>()
{
  uint8 s;
  *this >> s;
  return s;
}

template<>
inline sint8 IArchive::load<sint8>()
{
  sint8 s;
  *this >> s;
  return s;
}

template<>
inline uint16 IArchive::load<uint16>()
{
  uint16 s;
  *this >> s;
  return s;
}

template<>
inline sint16 IArchive::load<sint16>()
{
  sint16 s;
  *this >> s;
  return s;
}

template<>
inline uint32 IArchive::load<uint32>()
{
  uint32 s;
  *this >> s;
  return s;
}

template<>
inline sint32 IArchive::load<sint32>()
{
  sint32 s;
  *this >> s;
  return s;
}

template<>
inline String IArchive::load<String>()
{
  String s;
  *this >> s;
  return s;
}

template<typename T>
inline T IArchive::load()
{
  return T::load(*this);
}

}

#endif // IARCHIVE_H

