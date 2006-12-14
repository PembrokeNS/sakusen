#include "settingstree/stringlistleaf.h"

#include "server.h"

using namespace __gnu_cxx;

using namespace sakusen;
using namespace fuseki;
using namespace fuseki::settingsTree;

StringListLeaf::StringListLeaf(
    const String& name,
    const String& readers,
    const String& writers,
    Branch* parent,
    Server* server
  ) :
  Leaf(name, readers, writers, parent, server),
  value()
{
}

String StringListLeaf::setValue(const String& v)
{
  if (v.empty()) {
    return "When setting the value of a list leaf, the first character must "
        "be '0', '+' or '-'";
  }
  
  hash_set<String, StringHash> newValue = value;
  
  switch (v[0]) {
    case '0':
      if (v.length() > 1) {
        return "When clearing a list node, the value specified must be "
          "exactly '0'";
      }
      newValue.clear();
      break;
    case '+':
      {
        String adding = v.substr(1);
        if (newValue.count(adding)) {
          return "String to be added already in list";
        }
        newValue.insert(adding);
      }
      break;
    case '-':
      {
        String erasing = v.substr(1);
        if (0 == newValue.count(erasing)) {
          return "String to be removed not in list";
        }
        newValue.erase(erasing);
      }
      break;
    default:
      return "When setting the value of a list leaf, the first character must "
        "be '0', '+' or '-'";
  }

  String reason;
  if ("" != 
      (reason = server->stringSetSettingAlteringCallback(this, newValue))
    ) {
    return reason;
  }
  value = newValue;
  return "";
}

String StringListLeaf::getValue() const
{
  String result = "";
  for (__gnu_cxx::hash_set<String, StringHash>::const_iterator
      v = value.begin(); v != value.end(); v++) {
    result += "\n";
    result += *v;
  }
  return result;
}

