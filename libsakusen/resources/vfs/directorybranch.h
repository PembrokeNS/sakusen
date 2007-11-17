#ifndef LIBSAKUSEN_RESOURCES__VFS__DIRECTORYBRANCH_H
#define LIBSAKUSEN_RESOURCES__VFS__DIRECTORYBRANCH_H

#include "vfs/branch.h"

namespace sakusen {
namespace resources {
namespace vfs {

class DirectoryBranch : public Branch {
  public:
    typedef boost::shared_ptr<DirectoryBranch> Ptr;
    typedef boost::shared_ptr<const DirectoryBranch> ConstPtr;

    static Ptr create(
        const String& name,
        const boost::filesystem::path& directory,
        const Branch::Ptr& parent = Branch::Ptr()
      ) {
      Ptr p(new DirectoryBranch(name, directory, parent));
      p->ptrToThis = p;
      return p;
    }
  private:
    DirectoryBranch(
        const String& name,
        const boost::filesystem::path& d,
        const Branch::Ptr& parent
      );

    const boost::filesystem::path directory;
  public:
    virtual Branch::Ptr getSubBranch(const String& name);
    virtual Branch::Ptr createSubBranch(const String& name);
    virtual boost::tuple<Resource, ResourceSearchResult> getResource(
        const String& name,
        const String& extension
      );
    virtual Writer::Ptr getWriter(const String& name);
};

}}}

#endif // LIBSAKUSEN_RESOURCES__VFS__DIRECTORYBRANCH_H

