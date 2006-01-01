#ifndef SETTINGSUSER_H
#define SETTINGSUSER_H

#include <set>

#include "libsakusen-global.h"

namespace fuseki {
namespace settingsTree {

class Node;

}

/* This abstract class represents an object which can access or alter the
 * settings tree */
class SettingsUser {
  private:
    SettingsUser();
  protected:
    SettingsUser(const String& group);
      /* Initialize with given group, and "world" in groups */
    SettingsUser(const SettingsUser&);
  public:
    virtual ~SettingsUser() {}
  private:
    std::set<String> groups;

  public:
    inline bool hasGroup(const String& group) const {
      return groups.find(group) != groups.end();
    }
    bool hasGroupIn(const std::set<String>& groupsToCheck) const;
    inline void addGroup(const String& group) { groups.insert(group); }
    inline void removeGroup(const String& group) {
      assert(groups.count(group));
      groups.erase(group);
    }
    inline void clearGroups() { groups.clear(); }
    bool hasReadPermissionFor(const fuseki::settingsTree::Node* node) const;
    bool hasWritePermissionFor(const fuseki::settingsTree::Node* node) const;
};

}

#endif // SETTINGSUSER_H

