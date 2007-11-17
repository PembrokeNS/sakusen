#include "vfs/unionbranch.h"

using namespace std;

namespace sakusen {
namespace resources {
namespace vfs {

Branch::Ptr UnionBranch::getSubBranch(const String& name)
{
  assert(!name.empty());
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

boost::tuple<Resource, ResourceSearchResult> UnionBranch::getResource(
    const String& name,
    const String& extension
  )
{
  assert(!name.empty());
  /** \todo memoize? */
  if (name == ".." || name == ".") {
    return boost::make_tuple(Resource(), resourceSearchResult_error);
  }
  Resource candidate;
  /** \bug Doesn't resolve ambiguity properly (e.g. when exactly the same file
   * is in multiple branches, that's reported as ambiguous) */
  for (vector<Branch::Ptr>::const_iterator i=branches.begin();
      i!= branches.end(); ++i) {
    ResourceSearchResult result;
    Resource r;
    boost::tie(r, result) = (*i)->getResource(name, extension);
    switch (result) {
      case resourceSearchResult_success:
        if (candidate.getReader()) {
          return boost::make_tuple(Resource(), resourceSearchResult_ambiguous);
        } else {
          /*QDebug("got candidate");*/
          candidate = r;
        }
        break;
      case resourceSearchResult_ambiguous:
      case resourceSearchResult_notSupported:
        /* In these cases we propogate the result */
        return boost::make_tuple(Resource(), result);
        break;
      case resourceSearchResult_notFound:
      case resourceSearchResult_error:
        /* In these cases we try again elsewhere */
        break;
    }
  }
  
  if (candidate.getReader()) {
    return boost::make_tuple(candidate, resourceSearchResult_success);
  }
  
  return boost::make_tuple(Resource(), resourceSearchResult_notFound);
}

Writer::Ptr UnionBranch::getWriter(const String& name)
{
  return branches.front()->getWriter(name);
}

}}}

