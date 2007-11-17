#include "vfs/branch.h"

#include <deque>

using namespace std;

namespace sakusen {
namespace resources {
namespace vfs {

Branch::Branch(const String& n, const Ptr& p) : name(n), parent(p)
{
  if (parent.lock()) {
    root = parent.lock()->root;
  }
}

Branch::Ptr Branch::getRoot() const
{
  Ptr r = root.lock();
  if (r) {
    return r;
  }
  return ptrToThis.lock();
}

String Branch::getSakusenPath() const
{
  Ptr p = parent.lock();
  
  if (!p) {
    /* At top of resource tree */
    assert(name.empty());
    return "/";
  }

  assert(!name.empty());

  String parentPath = p->getSakusenPath();
  assert(!parentPath.empty());
  if (*parentPath.rbegin() == '/') {
    return parentPath + name;
  } else {
    return parentPath + "/" + name;
  }
}

Writer::Ptr Branch::openWriter(
    const String& sakusenPath
  ) {
  String::const_iterator afterSlash = sakusenPath.begin();
  while (afterSlash != sakusenPath.end() && *afterSlash == '/') {
    ++afterSlash;
  }
  bool rootedPath = (afterSlash != sakusenPath.begin());
  String relPath(afterSlash, sakusenPath.end());
  deque<String> splitPath = stringUtils_split<deque<String> >(relPath, "/");
  if (splitPath.empty()) {
    /** \todo Is it better to throw an exception or return null here? */
    Fatal("empty path");
  }
  Branch::Ptr currentBranch =
    ( rootedPath ? getRoot() : ptrToThis.lock() );
  while (splitPath.size() > 1) {
    String next = splitPath.front();
    splitPath.pop_front();
    currentBranch = currentBranch->createSubBranch(next);
  }

  String next = splitPath.front();
  splitPath.pop_front();
  return currentBranch->getWriter(next);
}

boost::tuple<Resource, ResourceSearchResult> Branch::search(
    const String& sakusenPath,
    const String& extension
  ) {
  /** \bug Duplicates code in openWriter */
  String::const_iterator afterSlash = sakusenPath.begin();
  while (afterSlash != sakusenPath.end() && *afterSlash == '/') {
    ++afterSlash;
  }
  bool rootedPath = (afterSlash != sakusenPath.begin());
  String relPath(afterSlash, sakusenPath.end());
  deque<String> splitPath = stringUtils_split<deque<String> >(relPath, "/");
  if (splitPath.empty()) {
    return boost::make_tuple(Resource(), resourceSearchResult_notFound);
  }
  Branch::Ptr currentBranch =
    ( rootedPath ? getRoot() : ptrToThis.lock() );
  while (splitPath.size() > 1) {
    String next = splitPath.front();
    splitPath.pop_front();
    currentBranch = currentBranch->getSubBranch(next);
    if (!currentBranch) {
      return boost::make_tuple(Resource(), resourceSearchResult_notFound);
    }
  }

  String next = splitPath.front();
  splitPath.pop_front();
  return currentBranch->getResource(next, extension);
}

}}}

