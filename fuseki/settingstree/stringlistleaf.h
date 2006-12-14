#ifndef STRINGLISTLEAF_H
#define STRINGLISTLEAF_H

#include "gnu_extensions.h"

#include "settingstree/leaf.h"

namespace fuseki {
namespace settingsTree {

class StringListLeaf : public Leaf {
  public:
    StringListLeaf(
        const String& name,
        const String& readers,
        const String& writers,
        Branch* parent,
        Server* server
      );
    virtual ~StringListLeaf() { }
  private:
    __gnu_cxx::hash_set<String, sakusen::StringHash> value;
  protected:
    virtual String setValue(const String& v);
    virtual String getValue() const;
  public:
    inline const __gnu_cxx::hash_set<String, sakusen::StringHash>&
      getValueAsList() const {
      return value;
    }
};

}}

#endif // STRINGLISTLEAF_H

