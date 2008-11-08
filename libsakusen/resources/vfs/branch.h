#ifndef LIBSAKUSEN_RESOURCES__VFS__BRANCH_H
#define LIBSAKUSEN_RESOURCES__VFS__BRANCH_H

#include <boost/filesystem/path.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include "reader.h"
#include "writer.h"
#include "resourcetype.h"
#include "resourcesearchresult.h"
#include "vfs/resource.h"

namespace sakusen {
namespace resources {
namespace vfs {

class Branch : private boost::noncopyable {
  public:
    typedef boost::shared_ptr<Branch> Ptr;
    typedef boost::shared_ptr<const Branch> ConstPtr;
  protected:
    Branch(const String& name, const Ptr& parent);
  public:
    virtual ~Branch() = 0;
  protected:
    /** \brief Name of just this branch */
    const String name;
    boost::weak_ptr<Branch> ptrToThis;
    const boost::weak_ptr<Branch> parent;
    /** \brief weak_ptr to the root of the resource hierarchy.
     *
     * Null if this is the root. */
    boost::weak_ptr<Branch> root;
  public:
    /** \brief Get the full sakusen path of this branch */
    String getSakusenPath() const;
    /** \brief Get the root of the resource hierarchy.
     * 
     * this is more useful than just using root, because it never yields null.
     * */
    Ptr getRoot() const;
    /** \brief Get a sub-branch of this one with name exactly \p name. 
     *
     * If no such sub-branch exists, return Ptr(). */
    virtual Ptr getSubBranch(const String& name) = 0;
    /** \brief Get a sub-branch of this one with name exactly \p name.
     * 
     * If no such sub-branch exists, create it. */
    virtual Ptr createSubBranch(const String& name) = 0;
    /** \brief Get all resources in this branch with name starting \p name and
     * ending \p extension. */
    virtual std::list<Resource> getResources(
        const String& name,
        const String& extension
      ) = 0;
    /** \brief Get a writer in this branch with name \p name.
     * 
     * Unlike openWriter, this method requires name *not* contain slashes. */
    virtual Writer::Ptr getWriter(const String& name) = 0;

    /** \brief Open a writer at path \p sakusenPath.
     *
     * This is strictly more general than getWriter, because it forwards to
     * getWriter in the even that sakusenPath contains no slashes. */
    Writer::Ptr openWriter(const String& sakusenPath);

    /** \brief Search for resource at path \p sakusenPath with name ending \p
     * extension.
     * 
     * All components of the path except the last much match exactly; the last
     * may be only a prefix.
     *
     * Success or failure returned through second part of return value. */
    boost::tuple<Resource, ResourceSearchResult> search(
        const String& sakusenPath,
        const String& extension
      );
};

inline Branch::~Branch() {}

}}}

#endif // LIBSAKUSEN_RESOURCES__VFS__BRANCH_H

