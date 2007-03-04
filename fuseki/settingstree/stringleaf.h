#ifndef STRINGLEAF_H
#define STRINGLEAF_H

#include "settingstree/leaf.h"

namespace fuseki {
namespace settingsTree {

class StringLeaf : public Leaf {
  public:
    StringLeaf(
        const String& name,
        const String& readers,
        const String& writers,
        Branch* parent,
        Server* server,
        const String& initialValue = ""
      );
    virtual ~StringLeaf() { }
  protected:
    String value;

    virtual String setValue(const String& v);
    virtual String getValue() const;
};

}}

#endif // STRINGLEAF_H

