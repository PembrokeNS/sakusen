#ifndef STRINGSETLEAF_H
#define STRINGSETLEAF_H

#include <set>

#include "settingstree/leaf.h"

namespace fuseki {
namespace settingsTree {

class StringSetLeaf : public Leaf {
  public:
    typedef boost::shared_ptr<StringSetLeaf> Ptr;
    typedef boost::shared_ptr<const StringSetLeaf> ConstPtr;
    
    StringSetLeaf(
        const String& name,
        const String& readers,
        const String& writers,
        Branch* parent,
        Server* server,
        const std::set<String>& initialValue = std::set<String>()
      );
    virtual ~StringSetLeaf() { }
  private:
    std::set<String> value;
  protected:
    virtual String setValue(const String& v);
    virtual String getValue() const;
  public:
    inline const std::set<String>& getValueAsSet() const {
      return value;
    }
};

}}

#endif // STRINGSETLEAF_H

