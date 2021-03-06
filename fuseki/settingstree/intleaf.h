#ifndef FUSEKI__SETTINGSTREE__INTLEAF_H
#define FUSEKI__SETTINGSTREE__INTLEAF_H

#include "leaf.h"

namespace fuseki {
namespace settingsTree {

/* This defines a leaf storing a value of type T, which is assumed to be an
 * integer type */
template <typename T>
class IntLeaf : public Leaf {
  SAKUSEN_STATIC_ASSERT((boost::is_integral<T>::value));
  SAKUSEN_STATIC_ASSERT((!boost::is_same<T, bool>::value));
  public:
    IntLeaf(
        const String& name,
        const String& readers,
        const String& writers,
        Branch* parent,
        Server* server,
        T value = 0
      );
    virtual ~IntLeaf() {}
  private:
    T value;
  protected:
    virtual String setValue(const String& v);
    virtual std::set<String> getValue() const;
  public:
    inline T getValueAsInt() const { return value; }
};

}}

#endif // FUSEKI__SETTINGSTREE__INTLEAF_H

