#include "settingstree/stringleaf.h"

#include "server.h"

using namespace fuseki::settingsTree;

StringLeaf::StringLeaf(
    const String& name,
    const String& readers,
    const String& writers,
    const Branch* parent,
    Server* server
  ) :
  Leaf(name, readers, writers, parent, server)
{
}

String StringLeaf::setValue(const String& v)
{
  String reason;
  if ("" != (reason = server->stringSettingAlteringCallback(this, v))) {
    return reason;
  }
  value = v;
  return "";
}

String StringLeaf::getValue() const
{
  return value;
}

