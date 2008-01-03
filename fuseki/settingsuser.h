#ifndef SETTINGSUSER_H
#define SETTINGSUSER_H

#include "fuseki-global.h"

#include <set>

#include <boost/shared_ptr.hpp>

namespace fuseki {
namespace settingsTree {

class Node;

}

/** \brief Abstract class representing an object which can access or alter the
 * settings tree
 *
 * This class keeps track of what groups a user of the settings tree belongs to
 * so that permissions can be calculated appropriately. */
class SettingsUser {
  private:
    SettingsUser();
  protected:
    /** \brief Standard constructor
     *
     * Initializes user as a member of the groups "world" and \a group. */
    SettingsUser(const String& group);
    SettingsUser(const SettingsUser&);
  public:
    /** \brief Destructor */
    virtual ~SettingsUser() {}
  private:
    std::set<String> groups;

  public:
    /** \return true iff this user belongs to the group \a group */
    inline bool hasGroup(const String& group) const {
      return groups.find(group) != groups.end();
    }
    /** \return true iff this user belongs to at least one of the groups in \a
     * groupsToCheck */
    bool hasGroupIn(const std::set<String>& groupsToCheck) const;
    /** \brief Adds a group to those of which this user is a member */
    inline void addGroup(const String& group) { groups.insert(group); }
    /** \brief Removes a group from those to which this user is a member */
    inline void removeGroup(const String& group) {
      assert(hasGroup(group));
      groups.erase(group);
    }
    /** \brief Clears all groups of which this user is a member */
    inline void clearGroups() { groups.clear(); }
    /** \return true iff this user has permission to read \a node */
    bool hasReadPermissionFor(
        boost::shared_ptr<const settingsTree::Node> node
      ) const;
    /** \return true iff this user has permission to write \a node */
    bool hasWritePermissionFor(
        boost::shared_ptr<const settingsTree::Node> node
      ) const;
};

}

#endif // SETTINGSUSER_H

