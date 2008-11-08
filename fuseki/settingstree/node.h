#ifndef NODE_H
#define NODE_H

#include <list>
#include <iosfwd>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/tuple/tuple.hpp>

#include "libsakusen-global.h"
#include "clientid.h"
#include "settingsuser.h"

namespace fuseki {

class Server;

namespace settingsTree {

class Branch;

class Node : private boost::noncopyable {
  public:
    typedef boost::shared_ptr<Node> Ptr;
    typedef boost::shared_ptr<const Node> ConstPtr;
  protected:
    Node(
        const String& name,
        const String& readers,
        const String& writers,
        Branch* parent,
        fuseki::Server* server
      ); /* both readers and writers are interpreted as a comma-seperated list
            of group names.  'server' is added automatically to both lists */
  public:
    virtual ~Node() {}
  private:
    String name;
    Branch* parent;
      /* node above this in the tree (NULL if this is the root node).
       * Not owned by this. */
    std::set<String> readingGroups; /* Groups with read permission */
    std::set<String> writingGroups; /* Groups with write permission */
  protected:
    fuseki::Server* server; /* The server whose tree this is */

    virtual Node::Ptr ptrToThis();
    virtual Node::ConstPtr ptrToThis() const;
  public:
    inline const String& getName() const { return name; }
    virtual bool isLeaf() const = 0;
    inline const std::set<String>& getReadingGroups() const {
      return readingGroups;
    }
    inline const std::set<String>& getWritingGroups() const {
      return writingGroups;
    }
    virtual Node::Ptr getNodeByListRef(
        std::list<String>& nodeAddress
      ) = 0; /* Note: alters its argument nodeAddress */
    inline Node::Ptr getNodeByList(
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
    virtual boost::tuple<String, std::set<String>, Node::ConstPtr>
      getRequestListRef(
        std::list<String>& nodeAddress,
        const SettingsUser* user
      ) const = 0; /* Note: alters its argument nodeAddress */
    inline boost::tuple<String, std::set<String>, Node::ConstPtr>
      getRequestList(
        std::list<String> nodeAddress,
        const SettingsUser* user
      ) const { return getRequestListRef(nodeAddress, user); }
};

}}

#endif // NODE_H

