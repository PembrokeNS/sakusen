#include <sakusen/resources/vfs/unionbranch.h>

#include <boost/foreach.hpp>

using namespace std;

namespace sakusen {
namespace resources {
namespace vfs {

Branch::Ptr UnionBranch::getSubBranch(const String& name)
{
  if (name.empty()) {
    return Branch::Ptr();
  }

  /** \todo memoize? */
  if (name == "..") {
    return parent.lock();
  }
  if (name == ".") {
    return ptrToThis.lock();
  }
  vector<Branch::Ptr> subBranches;
  for (vector<Branch::Ptr>::const_iterator i=branches.begin();
      i!= branches.end(); ++i) {
    Branch::Ptr subBranch = (*i)->getSubBranch(name);
    if (subBranch) {
      subBranches.push_back(subBranch);
    }
  }
  
  if (subBranches.empty()) {
    return Branch::Ptr();
  }
  return create(name, subBranches, ptrToThis.lock());
}

Branch::Ptr UnionBranch::createSubBranch(const String& name)
{
  assert(!name.empty());
  if (name == "..") {
    return parent.lock();
  }
  if (name == ".") {
    return ptrToThis.lock();
  }
  /* We assume that we're only allowed to create subbranches in the first of
   * our branches.  But!  We also need to throw in all parallel branches in
   * order to maintain proper union-ness. */
  assert(!branches.empty());
  vector<Branch::Ptr> subBranches;
  subBranches.push_back(branches.front()->createSubBranch(name));
  for (vector<Branch::Ptr>::const_iterator i=branches.begin()+1;
      i!= branches.end(); ++i) {
    Branch::Ptr subBranch = (*i)->getSubBranch(name);
    if (subBranch) {
      subBranches.push_back(subBranch);
    }
  }
  
  return create(name, subBranches, ptrToThis.lock());
}

list<Resource> UnionBranch::getResources(
    const String& name,
    const String& extension
  )
{
  /** \todo memoize? */
  if (name.empty() || name == ".." || name == ".") {
    throw DeserializationExn("Invalid resource name '"+name+"'");
  }
  list<Resource> candidates;
  /** \bug Doesn't resolve ambiguity properly (e.g. when exactly the same file
   * is in multiple branches, that's reported as ambiguous) */
  BOOST_FOREACH (Branch::Ptr branch, branches) {
    list<Resource> branchCandidates = branch->getResources(name, extension);
    candidates.splice(candidates.end(), branchCandidates);
  }
  
  return candidates;
}

Writer::Ptr UnionBranch::getWriter(const String& name)
{
  return branches.front()->getWriter(name);
}

}}}

