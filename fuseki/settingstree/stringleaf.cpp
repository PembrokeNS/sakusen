#include "settingstree/stringleaf.h"

#include "server.h"

using namespace fuseki;
using namespace fuseki::settingsTree;

StringLeaf::StringLeaf(
    const String& name,
    const String& readers,
    const String& writers,
    Branch* parent,
    Server* server,
    const String& initialValue
  ) :
  Leaf(name, readers, writers, parent, server),
  value(initialValue)
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

