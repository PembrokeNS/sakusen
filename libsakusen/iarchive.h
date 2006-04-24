#ifndef IARCHIVE_H
#define IARCHIVE_H

#include "libsakusen-global.h"

#include <vector>
#include <list>

#include "point.h"
#include "exceptions.h"
#include "stringutils.h"

namespace sakusen {

/* This class represents an archive from which we extract objects after
 * receiving it across the network or reading it from disk.  c.f. OArchive.
 * Thus it can be constructed from a buffer of uint8s and has lots of overloads
 * of operator>>.
 * It's worth noting that the libstdc++ docs say that doing precisely this is a
 * Bad Thing, because the << and >> operators should only be used for formatted
 * input and output.  The correct solution is make these classes ([IO]Archive)
 * subclasses of streambuf, but I don't know how to do that.
 * Besides that, we have the problem of differnt floating point formats (solved
 * at present by storing such numbers as text).
 * TODO: deal with all of the above.
 * The name IArchive follows the istream/ostream convention */
class LIBSAKUSEN_API IArchive {
  private:
    IArchive();
    IArchive(const IArchive&);
  public:
    IArchive(const uint8* buffer, size_t length);
    ~IArchive();
  private:
    uint8* originalBuffer; /* owned by this */
    const uint8* buffer;
    size_t remainingLength;
    inline void assertLength(size_t length) const {
      /* We ensure that enough length remains.
       * If not, we throw an exception */
      if (remainingLength < length) {
        throw new EndOfArchiveDeserializationExn();
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
  public:
    inline String getSecureHashAsString() const {
      return stringUtils_getSecureHashAsString(
          originalBuffer, (buffer+remainingLength)-originalBuffer
        );
    }
    inline bool isFinished() const { return remainingLength == 0; }
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

    template<typename T>
    IArchive& extractEnum(T& result)
    {
      uint8 valAsInt;
      *this >> valAsInt;
      result = static_cast<T>(valAsInt);
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
    
    template<typename T, int size>
    IArchive& extract(T result[size])
    {
      for (int i=0; i<size; ++i) {
        result[i]=load<T>();
      }

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
          throw new DeserializationExn("Duplicate key in hash_map");
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

    template<int size>
    inline void magicValue(const char val[size+1]) {
      /* The '+1' in the array length is to allow for the trailing null char */
      assertLength(size);
      if (0 != memcmp(val, buffer, size)) {
        Debug("Wrong magic");
        throw new WrongMagicDeserializationExn(val, size, buffer);
      }
      advance(size);
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

