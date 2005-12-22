#include "settingsuser.h"

#include "settingstree/node.h"

using namespace fuseki;
using namespace fuseki::settingsTree;

SettingsUser::SettingsUser(const String& group) :
  groups()
{
  groups.insert(group);
  groups.insert("world");
}

bool SettingsUser::hasGroupIn(const std::set<String>& groupsToCheck) const
{
  /* We have to determine whether the two sets groups and groupsToCheck have an
   * intersection.  I thought there would be an STL algorithm for this, but
   * here doesn't appear to be.  There is the "setintersection" algorithm, but
   * that won't abort when it finds an intersection.  So, we do it "by hand" */
  
  std::set<String>::const_iterator i1 = groups.begin();
  std::set<String>::const_iterator i2 = groupsToCheck.begin();

  while (true) {
    if (i1 == groups.end()) {
      return false;
    } else if (i2 == groupsToCheck.end()) {
      return false;
    } else if (*i1 == *i2) {
      return true;
    } else if (*i1 < *i2) {
      ++i1;
    } else {
      ++i2;
    }
  }
}

bool SettingsUser::hasReadPermissionFor(const Node* node) const
{
  return hasGroupIn(node->getReadingGroups());
}

bool SettingsUser::hasWritePermissionFor(const Node* node) const
{
  return hasGroupIn(node->getWritingGroups());
}

