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
#include "deserializationcontext.h"
#include "buffer.h"

namespace sakusen {

class OArchive;

/** \internal \brief Class used inside IArchive for working with
 * boost::multi_array */  
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

/** \internal \brief Helper class to handle extraction of different types
 * from an IArchive. */
template<typename T>
struct Extracter {
  T operator()(IArchive&);
  T operator()(IArchive&, const DeserializationContext& context);
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
 * Besides that, we have the problem of different floating point formats (solved
 * at present by storing such numbers as text).
 *
 * \see OArchive
 * \todo deal with all of the above
 *
 * \note The name IArchive follows the istream/ostream convention.
 */
class LIBSAKUSEN_API IArchive : boost::noncopyable {
  friend class OArchive;
  public:
    IArchive();
    IArchive(const uint8* buffer, size_t length);
    IArchive(const boost::shared_array<uint8>& buffer, size_t length);
    IArchive(const boost::shared_array<const uint8>& buffer, size_t length);
    IArchive(const Buffer& buffer);
  private:
    const Buffer originalBuffer;
    /* Pointer into originalBuffer's memory, which is safe only because
     * originalBuffer is const, and thus will never copy its memory */
    const uint8* pos; 
    size_t remainingLength;
    inline void assertLength(size_t length) const {
      /* We ensure that enough length remains.
       * If not, we throw an exception */
      if (remainingLength < length) {
        throw EndOfArchiveDeserializationExn();
      }
    }
    inline void advance(size_t length) {
      pos += length;
      remainingLength -= length;
    }
  public:
    /** \brief Get an ASCII hex version of a hash of the contents of the
     * IArchive as at time of construction.
     *
     * This function should continue to return the same thing, even when things
     * are extracted from the IArchive. */
    inline String getSecureHashAsString() const {
      return originalBuffer.getSecureHashAsString();
    }
    /** \brief Determine whether the IArchive is exhausted, with no more data
     * to extract */
    inline bool isFinished() const { return remainingLength == 0; }
    void dumpBuffer() const;
    inline IArchive& operator>>(bool& i) {
      assertLength(sizeof(uint8));
      assert(*pos <= 1);
      i = !(0 == *pos);
      advance(sizeof(uint8));
      return *this;
    }
    inline IArchive& operator>>(uint8& i) {
      assertLength(sizeof(uint8));
      i = *pos;
      advance(sizeof(uint8));
      return *this;
    }
    inline IArchive& operator>>(sint8& i) {
      assertLength(sizeof(sint8));
      i = *pos;
      advance(sizeof(sint8));
      return *this;
    }
    IArchive& operator>>(uint16& i);
    IArchive& operator>>(sint16& i);
    IArchive& operator>>(uint32& i);
    IArchive& operator>>(sint32& i);
    IArchive& operator>>(uint64& i);
    IArchive& operator>>(sint64& i);
    IArchive& operator>>(double& d);
    IArchive& operator>>(String& s);
    IArchive& operator>>(Buffer&);

    /** \brief Extract an enum value from the IArchive
     *
     * It is important to use this method to extract enum values rather than
     * using operator>> because the underlying integer type of an enum is
     * platform-dependent.  As an added bonus, it saves the caller from having
     * to static_cast too.
     *
     * \warning This assumes that the enum values all fall in the range 0-255
     */
    template<typename T>
    IArchive& extractEnum(T& result) {
      uint8 valAsInt;
      *this >> valAsInt;
      result = static_cast<T>(valAsInt);
      return *this;
    }

    template<typename T, typename U>
    IArchive& operator>>(std::pair<T, U>& result) {
      result.first = Extracter<T>()(*this);
      result.second = Extracter<U>()(*this);
      return *this;
    }

    template<typename TInteger, typename TBase>
    IArchive& operator>>(IdBase<TInteger, TBase>& result) {
      TInteger v;
      *this >> v;
      result.val = v;
      return *this;
    }
    
    template<typename T, size_t size>
    IArchive& extract(T result[size]) {
      Extracter<T> extracter;
      for (int i=0; i<size; ++i) {
        result[i]=extracter(*this);
      }

      return *this;
    }
    
    template<typename T, size_t size>
    IArchive& extract(T (&result)[size], const DeserializationContext& context)
    {
      Extracter<T> extracter;
      for (size_t i=0; i<size; ++i) {
        result[i]=extracter(*this, context);
      }

      return *this;
    }
    
    template<typename T>
    IArchive& extract(T* result, size_t size) {
      Extracter<T> extracter;
      for (size_t i=0; i<size; ++i) {
        result[i] = extracter();
      }

      return *this;
    }
    
    template<typename T, size_t size>
    IArchive& extract(boost::array<T, size>& result) {
      Extracter<T> extracter;
      for (size_t i=0; i<size; ++i) {
        result[i]=extracter(*this);
      }

      return *this;
    }

    template<typename T, size_t rank>
    IArchive& extract(boost::multi_array<T, rank>& result) {
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
      Extracter<T> extracter;

      do {
        result(i) = extracter(*this);
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
    IArchive& operator>>(std::vector<T>& result) {
      assert(result.empty());
      uint32 size;
      *this >> size;
      Extracter<T> extracter;

      while (size--) {
        result.push_back(extracter(*this));
      }

      return *this;
    }

    template<typename T>
    IArchive& operator>>(std::list<T>& result) {
      assert(result.empty());
      uint32 size;
      *this >> size;
      Extracter<T> extracter;

      while (size--) {
        result.push_back(extracter(*this));
      }

      return *this;
    }

    template<typename T, typename U, typename THash>
    IArchive& operator>>(__gnu_cxx::hash_map<T, U, THash>& result) {
      assert(result.empty());
      uint32 size;
      *this >> size;
      Extracter<std::pair<T, U> > extracter;

      while (size--) {
        if (!result.insert(extracter(*this)).second) {
          throw DeserializationExn("Duplicate key in hash_map");
        }
      }

      return *this;
    }

    template<typename T>
    IArchive& extract(
        std::vector<T>& result,
        const DeserializationContext& context
      ) {
      assert(result.empty());
      uint32 size;
      *this >> size;
      Extracter<T> extracter;

      while (size--) {
        result.push_back(extracter(*this, context));
      }

      return *this;
    }

    template<typename T>
    IArchive& extract(
        std::list<T>& result,
        const DeserializationContext& context
      ) {
      assert(result.empty());
      uint32 size;
      *this >> size;
      Extracter<T> extracter;

      while (size--) {
        result.push_back(extracter(*this, context));
      }

      return *this;
    }

    template<typename T>
    IArchive& operator>>(Point<T>& result) {
      T x;
      *this >> x;
      T y;
      *this >> y;
      T z;
      *this >> z;

      result = Point<T>(x,y,z);
      return *this;
    }

    /** \brief Tests for a magic value in the archive at this point.
     *
     * As an aid to catching errors and debugging, magic values can be inserted
     * into an archive with OArchive::magicValue.  At extraction time this
     * method should be called to confirm that the magic value is correct and
     * step over it.  If the magic value does not match, an exception is
     * thrown. */
    inline void magicValue(const String& val) {
      assertLength(val.size());
      if (0 != memcmp(val.c_str(), pos, val.size())) {
        Debug("Wrong magic");
        throw WrongMagicDeserializationExn(val, pos);
      }
      advance(val.size());
    }
};

#define SIMPLE_EXTRACTER(type) \
template<>                     \
struct Extracter<type> {       \
  type operator()(             \
      IArchive& archive        \
    )                          \
  {                            \
    type item;                 \
    archive >> item;           \
    return item;               \
  }                            \
};

SIMPLE_EXTRACTER(uint8)
SIMPLE_EXTRACTER(sint8)
SIMPLE_EXTRACTER(uint16)
SIMPLE_EXTRACTER(sint16)
SIMPLE_EXTRACTER(uint32)
SIMPLE_EXTRACTER(sint32)
SIMPLE_EXTRACTER(uint64)
SIMPLE_EXTRACTER(sint64)
SIMPLE_EXTRACTER(String)

#undef SIMPLE_EXTRACTER

template<typename T, typename U>
struct Extracter<std::pair<T, U> > {
  std::pair<T, U> operator()(IArchive& archive)
  {
    std::pair<T, U> item;
    archive >> item;
    return item;
  }
};

template<typename T>
struct Extracter<std::vector<T> > {
  std::vector<T> operator()(IArchive& archive)
  {
    std::vector<T> item;
    archive >> item;
    return item;
  }
};

template<typename T>
struct Extracter<Point<T> > {
  Point<T> operator()(IArchive& archive)
  {
    Point<T> item;
    archive >> item;
    return item;
  }
};

template<typename T>
inline T Extracter<T>::operator()(IArchive& archive)
{
  return T::load(archive);
}

template<typename T>
inline T Extracter<T>::operator()(
    IArchive& archive,
    const DeserializationContext& context
  )
{
  return T::load(archive, context);
}

}

#endif // IARCHIVE_H

