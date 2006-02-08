#ifndef IARCHIVE_H
#define IARCHIVE_H

#include "libsakusen-global.h"

#include <vector>
#include <list>

#include "point.h"
#include "exceptions.h"

namespace sakusen {

/* This class represents an archive from which we extract objects after
 * receiving it across the network or reading it from disk.  c.f. OArchive.
 * Thus it can be constructed from a buffer of uint8s and has lots of overloads
 * of operator>>.
 * It's worth noting that the libstdc++ docs say that doing precisely this is a
 * Bad Thing, because the << and >> operators should only be used for formatted
 * input and output.  The correct solution is make these classes ([IO]Archive)
 * inheritors from streambuf, but I don't know how to do that.
 * Besides that, we have the problems of endianess and bit patterns
 * (particularly for floating point types) which mean that it might be better
 * to textify numbers rather than writing them as binary.  At the very least we
 * need to get an error on machines with the "wrong" endianess (although that
 * requires using the autotools).
 * TODO: deal with all of the above diatrabe.
 * The name IArchive foolows the istream/ostream convention */
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
    inline T load()
    {
      return T::load(*this);
    }

    template<typename T>
    inline T load(const typename T::loadArgument* arg)
    {
      return T::load(*this, arg);
    }
  public:
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
};
    
template<>
inline String IArchive::load<String>()
{
  String s;
  *this >> s;
  return s;
}

}

#endif // IARCHIVE_H

