#include "settingstree/node.h"

#include "libsakusen-comms-global.h"
#include "stringutils.h"

#include <sstream>

using namespace std;

using namespace sakusen;
using namespace fuseki;
using namespace fuseki::settingsTree;

Node::Node(
    const String& n,
    const String& readers,
    const String& writers,
    const Node* p,
    Server* s
  ) :
  name(n),
  parent(p),
  readingGroups(),
  writingGroups(),
  server(s)
{
  assert((p == NULL) == n.empty());
  assert(s != NULL);

  readingGroups.insert("server");
  writingGroups.insert("server");

  list<String> readerList = stringUtils_split(readers, ",");

  for (list<String>::iterator reader = readerList.begin();
      reader != readerList.end(); reader++) {
    readingGroups.insert(*reader);
  }

  list<String> writerList = stringUtils_split(writers, ",");

  for (list<String>::iterator writer = writerList.begin();
      writer != writerList.end(); writer++) {
    writingGroups.insert(*writer);
  }
}

Node::~Node()
{
}

String Node::getFullName() const
{
  ostringstream nameStream;
  streamFullName(nameStream);
  return nameStream.str();
}

void Node::appendFullNameAsList(std::list<String>& fullName) const
{
  if (parent != NULL) {
    parent->appendFullNameAsList(fullName);
  }
  if (!name.empty()) {
    fullName.push_back(name);
  }
}

list<String> Node::getFullNameAsList() const
{
  list<String> fullName;
  appendFullNameAsList(fullName);
  return fullName;
}

void Node::streamFullName(ostream& nameStream) const
{
  if (parent != NULL) {
    parent->streamFullName(nameStream);
  }
  if (!name.empty()) {
    nameStream << SETTINGS_DELIMITER << name;
  }
}

