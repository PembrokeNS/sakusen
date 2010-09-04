#ifndef LIBSAKUSEN_RESOURCES__VFS__UNIONBRANCH_H
#define LIBSAKUSEN_RESOURCES__VFS__UNIONBRANCH_H

#include "vfs/branch.h"

namespace sakusen {
namespace resources {
namespace vfs {

class UnionBranch : public Branch {
  public:
    typedef boost::shared_ptr<UnionBranch> Ptr;
    typedef boost::shared_ptr<UnionBranch> ConstPtr;

    template<typename Container>
    static Ptr create(
        const String& name,
        const Container& branches,
        const Branch::Ptr& parent = Branch::Ptr()
      ) {
      Ptr p(new UnionBranch(name, branches, parent));
      p->ptrToThis = p;
      return p;
    }
  private:
    template<typename Container>
    UnionBranch(
        const String& name,
        const Container& b,
        const Branch::Ptr& parent
      ) :
      Branch(name, parent),
      branches(b.begin(), b.end())
    {
      assert(!branches.empty());
    }

    const std::vector<Branch::Ptr> branches;
  public:
    virtual Branch::Ptr getSubBranch(const String& name);
    virtual Branch::Ptr createSubBranch(const String& name);
    virtual std::list<Resource> getResources(
        const String& name,
        const String& extension
      );
    virtual Writer::Ptr getWriter(const String& name);
};

}}}

#endif // LIBSAKUSEN_RESOURCES__VFS__UNIONBRANCH_H

