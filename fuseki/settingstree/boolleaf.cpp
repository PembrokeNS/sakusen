#include "settingstree/boolleaf.h"

#include "server.h"

using namespace fuseki;
using namespace fuseki::settingsTree;

BoolLeaf::BoolLeaf(
    const String& name,
    bool v,
    const String& readers,
    const String& writers,
    const Branch* parent,
    Server* server
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
      if ("" != (reason = server->boolSettingAlteringCallback(this, true))) {
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
      if ("" != (reason = server->boolSettingAlteringCallback(this, false))) {
        return reason;
      }
      
      value = false;
      return "";
    }
  }
  return "invalid value for boolean leaf";
}

String BoolLeaf::getValue() const
{
  if (value) {
    return "true";
  } else {
    return "false";
  }
}

