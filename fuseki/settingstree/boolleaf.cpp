#include "settingstree/boolleaf.h"

#include "server-methods.h"

using namespace fuseki;
using namespace fuseki::settingsTree;

BoolLeaf::BoolLeaf(
    const String& name,
    const String& readers,
    const String& writers,
    Branch* parent,
    Server* server,
    bool v
  ) :
  Leaf(name, readers, writers, parent, server),
  value(v)
{
}

String BoolLeaf::setValue(const String& v)
{
  if (v == "true" || v == "1" || v == "on") {
    if (value) {
      return "setting already has that value";
    } else {
      String reason;
      if ("" != (reason = server->settingAlteringCallback<bool>(this, true))) {
        return reason;
      }

      value = true;
      return "";
    }
  } else if (v == "false" || v == "0" || v == "off") {
    if (!value) {
      return "setting already has that value";
    } else {
      String reason;
      if ("" != (reason = server->settingAlteringCallback<bool>(this, false))) {
        return reason;
      }

      value = false;
      return "";
    }
  }
  return "invalid value '"+v+"' for boolean leaf";
}

std::set<String> BoolLeaf::getValue() const
{
  std::set<String> result;
  if (value) {
    result.insert("true");
  } else {
    result.insert("false");
  }
  return result;
}

