#ifndef STRINGLISTLEAF_H
#define STRINGLISTLEAF_H

#include "gnu_extensions.h"

#include "settingstree/leaf.h"

namespace fuseki {
namespace settingsTree {

class StringListLeaf : public Leaf {
  private:
    StringListLeaf();
    StringListLeaf(const StringListLeaf&);
  public:
    StringListLeaf(
        const String& name,
        const String& readers,
        const String& writers,
        const Branch* parent,
        Server* server
      );
    virtual ~StringListLeaf() { }
  private:
    __gnu_cxx::hash_set<String, StringHash> value;
  protected:
    virtual String setValue(const String& v);
    virtual String getValue() const;
  public:
    inline const __gnu_cxx::hash_set<String, StringHash>&
      getValueAsList() const {
      return value;
    }
};

}}

#endif // STRINGLISTLEAF_H

