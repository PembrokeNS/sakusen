#ifndef LIBSAKUSEN__BUFFER_H
#define LIBSAKUSEN__BUFFER_H

#include <boost/shared_array.hpp>
#include <boost/call_traits.hpp>
#include <boost/type_traits/is_integral.hpp>

#include "stringutils.h"

namespace sakusen {

/** \brief Buffer is a raw byte stream implementing copy-on-write */
class Buffer {
  public:
    explicit Buffer(size_t s = 0, size_t c = size_t(-1)) :
      size(s),
      capacity(c == size_t(-1) ? s : c),
      content()
    {
      if (capacity) {
        content.reset(new uint8[capacity]);
      }
    }
    Buffer(const uint8* p, size_t l) : size(0), capacity(0), content()
    {
      appendBytes(p, l);
    }
    Buffer(const boost::shared_array<uint8>& p, size_t l) :
      size(0), capacity(0), content()
    {
      appendBytes(p, l);
    }
    Buffer(const boost::shared_array<const uint8>& p, size_t l) :
      size(0), capacity(0), content()
    {
      appendBytes(p, l);
    }
  private:
    size_t size;
    size_t capacity;
    boost::shared_array<uint8> content;

    void ensureCapacity(size_t reqdCapacity) {
      ensureUnique(reqdCapacity);
      if (capacity < reqdCapacity) {
        capacity = std::max(2*capacity, reqdCapacity);
        boost::shared_array<uint8> newContent(new uint8[capacity]);
        memcpy(newContent.get(), content.get(), size);
        content = newContent;
      }
    }

    void ensureSpace(size_t space) {
      ensureCapacity(size+space);
    }
  public:
    /** \brief If content is shared with another Buffer, make a copy for
     * ourselves.
     *
     * \param newCapacity If a reallocation is triggered, use at least this
     * many bytes. */
    void ensureUnique(size_t newCapacity=0) {
      if (!content || content.unique()) {
        return;
      }
      capacity = std::max(size, newCapacity);
      boost::shared_array<uint8> newContent(new uint8[capacity]);
      memcpy(newContent.get(), content.get(), size);
      content = newContent;
    }

    /** \brief Get direct access to content.
     *
     * \warning This method may cause a copy of the data to be made.  Use
     * getConst() if you don't need a non-const pointer.  Can't call both
     * methods the same because then there is no way to get a const pointer on
     * a non-const buffer. */
    uint8* get() {
      ensureUnique();
      return content.get();
    }

    /** \brief Get direct access to content.
     * */
    const uint8* getConst() const {
      return content.get();
    }

    size_t getSize() const { return size; }

    void resize(size_t newSize) {
      ensureCapacity(newSize);
      size = newSize;
    }

    void appendByte(const uint8 b) {
      ensureSpace(sizeof(uint8));
      content[size++] = b;
    }

    void appendBytes(const uint8* bytes, size_t len) {
      ensureSpace(len);
      memcpy(content.get()+size, bytes, len);
      size += len;
    }

    void appendBytes(const char* bytes, size_t len) {
      ensureSpace(len);
      memcpy(content.get()+size, bytes, len);
      size += len;
    }

    void appendBytes(const boost::shared_array<uint8>& bytes, size_t len) {
      ensureSpace(len);
      memcpy(content.get()+size, bytes.get(), len);
      size += len;
    }

    void appendBytes(const boost::shared_array<const uint8>&bytes,size_t len) {
      ensureSpace(len);
      memcpy(content.get()+size, bytes.get(), len);
      size += len;
    }

    /** \brief Appends the raw bytes of the parameter t to the buffer.
     *
     * Gives compile-time error if T is not an integral type (other types are
     * unlikely to be usefully passed to this function).
     *
     * Use with caution. It is wise to explicitly give the template parameter
     * so as to avoid accidentally passing t as the wrong type. */
    template<typename T>
    void appendRaw(typename boost::call_traits<T>::param_type t) {
      BOOST_STATIC_ASSERT(boost::is_integral<T>::value);
      appendBytes(
          reinterpret_cast<const uint8*>(boost::addressof(t)), sizeof(T)
        );
    }

    String getSecureHashAsString() const {
      return stringUtils_getSecureHashAsString(content.get(), size);
    }
};

}

#endif // LIBSAKUSEN__BUFFER_H

