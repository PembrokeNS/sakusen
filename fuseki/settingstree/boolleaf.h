#ifndef BOOLLEAF_H
#define BOOLLEAF_H

#include "settingstree/leaf.h"

namespace fuseki {
namespace settingsTree {

class BoolLeaf : public Leaf {
  public:
    BoolLeaf(
        const String& name,
        const String& readers,
        const String& writers,
        Branch* parent,
        Server* server,
        bool value = false
      );
    virtual ~BoolLeaf() {}
  private:
    bool value;
  protected:
    virtual String setValue(const String& v);
    virtual String getValue() const;
  public:
    inline bool getValueAsBool() const { return value; }
};

}}

#endif // BOOLLEAF_H

