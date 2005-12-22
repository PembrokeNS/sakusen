#ifndef NODE_H
#define NODE_H

#include <list>
#include <iosfwd>

#include "libsakusen-global.h"
#include "clientid.h"
#include "settingsuser.h"

namespace fuseki {

class Server;

namespace settingsTree {

class Node {
  private:
    Node();
    Node(const Node&);
  protected:
    Node(
        const String& name,
        const String& readers,
        const String& writers,
        const Node* parent,
        fuseki::Server* server
      ); /* both readers and writers are interpreted as a comma-seperated list
            of group names.  'server' is added automatically to both lists */
  public:
    virtual ~Node();
  private:
    String name;
    const Node* parent;
      /* node above this in the tree (NULL if this is the root node).
       * Not owned by this. */
    std::set<String> readingGroups; /* Groups with read permission */
    std::set<String> writingGroups; /* Groups with write permission */
  protected:
    fuseki::Server* server; /* The server whose tree this is */
  public:
    inline const String& getName() const { return name; }
    inline const std::set<String>& getReadingGroups() const {
      return readingGroups;
    }
    inline const std::set<String>& getWritingGroups() const {
      return writingGroups;
    }
    virtual Node* getNodeByListRef(
        std::list<String>& nodeAddress
      ) = 0; /* Note: alters its argument nodeAddress */
    inline Node* getNodeByList(
        std::list<String> nodeAddress
      ) { return getNodeByListRef(nodeAddress); }
    virtual void streamFullName(std::ostream& nameStream) const;
    virtual String getFullName() const;
    virtual void appendFullNameAsList(std::list<String>&) const;
    virtual std::list<String> getFullNameAsList() const;
    virtual String changeRequestListRef(
        std::list<String>& nodeAddress,
        const String& value,
        const SettingsUser* user
      ) = 0; /* Note: alters its argument nodeAddress */
    inline String changeRequestList(
        std::list<String> nodeAddress,
        const String& value,
        const SettingsUser* user
      ) { return changeRequestListRef(nodeAddress, value, user); }
    virtual String getRequestListRef(
        std::list<String>& nodeAddress,
        String& value,
        const SettingsUser* user
      ) const = 0; /* Note: alters its arguments nodeAddress, value */
    inline String getRequestList(
        std::list<String> nodeAddress,
        String& value,
        const SettingsUser* user
      ) const { return getRequestListRef(nodeAddress, value, user); }
};

}}

#endif // NODE_H

