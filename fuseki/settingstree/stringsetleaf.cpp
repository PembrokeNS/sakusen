#include "settingstree/stringsetleaf.h"

#include "server-methods.h"

using namespace std;
using namespace __gnu_cxx;

using namespace sakusen;
using namespace fuseki;
using namespace fuseki::settingsTree;

StringSetLeaf::StringSetLeaf(
    const String& name,
    const String& readers,
    const String& writers,
    Branch* parent,
    Server* server,
    const std::set<String>& initialValue
  ) :
  Leaf(name, readers, writers, parent, server),
  value(initialValue)
{
}

String StringSetLeaf::setValue(const String& v)
{
  if (v.empty()) {
    return "When setting the value of a list leaf, the first character must "
        "be '0', '+' or '-'";
  }
  
  set<String> newValue = value;
  
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
      (reason = server->settingAlteringCallback(this, newValue))
    ) {
    return reason;
  }
  value = newValue;
  return "";
}

set<String> StringSetLeaf::getValue() const
{
  return value;
}

