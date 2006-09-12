#ifndef MASKEDPTR_H
#define MASKEDPTR_H

template<typename T>
class MaskedPtrHash;

/** \brief Hides a pointer so that it cannot be dereferenced, but can still be
 * used as a UID.
 *
 * Beware, because although addresses serve as good UIDs for things that exist,
 * they can be swiftly reused after an object is deleted.
 */
template<typename T>
class MaskedPtr {
  friend class MaskedPtrHash<T>;
  public:
    MaskedPtr(const T& t) : ptr(&t) {}
    MaskedPtr(const T* p) : ptr(p) {}
    bool operator==(const MaskedPtr& right) const { return ptr == right.ptr; }
  private:
    const T* ptr;
};

template<typename T>
class MaskedPtrHash {
  public:
    inline size_t operator()(const MaskedPtr<T>& p) const {
      return reinterpret_cast<size_t>(p.ptr);
    }
};

#endif // MASKEDPTR_H

