#ifndef INTLEAF_H
#define INTLEAF_H

#include "leaf.h"

namespace fuseki {
namespace settingsTree {

/* This defines a leaf storing a value of type T, which is assumed to be an
 * integer type */
template <typename T>
class IntLeaf : public Leaf {
  public:
    IntLeaf(
        const String& name,
        T value,
        const String& readers,
        const String& writers,
        Branch* parent,
        Server* server
      );
    virtual ~IntLeaf() {}
  private:
    T value;
  protected:
    virtual String setValue(const String& v);
    virtual String getValue() const;
  public:
    inline T getValueAsInt() const { return value; }
};

}}

#endif // INTLEAF_H

