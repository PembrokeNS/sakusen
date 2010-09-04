#ifndef LIBSAKUSEN_RESOURCES__VFS__RESOURCE_H
#define LIBSAKUSEN_RESOURCES__VFS__RESOURCE_H

#include <sakusen/resources/fileioexn.h>

namespace sakusen {
namespace resources {
namespace vfs {

/** \brief A resource found somewhere in the VFS. */
class Resource {
  public:
    Resource() {}
    Resource(
        const String& sP,
        const Reader::Ptr& r,
        const boost::filesystem::path& p = boost::filesystem::path()
      ) :
      sakusenPath(sP),
      reader(r),
      path(p)
    {}
  private:
    String sakusenPath;
    Reader::Ptr reader;
    boost::filesystem::path path;
  public:
    operator bool() const { return reader; }
    /** \brief Get full sakusen path to this resource */
    String getSakusenPath() const { return sakusenPath; }
    /** \brief Get a Reader to the content of this resource */
    Reader::Ptr getReader() const { return reader; }
    /** \brief Get a path to a real on-disk file for this resource.
     *
     * This could be necessary, for example, when opening the item as a
     * module.
     *
     * \todo For resources that are not a file already, we may need to copy the
     * content to a temporary file, but for now just throw an exception */
    boost::filesystem::path asPath() const {
      if (path.empty()) {
        throw FileIOExn::customMessage("Not a real file");
      } else {
        return path;
      }
    }
};

}}}

#endif // LIBSAKUSEN_RESOURCES__VFS__RESOURCE_H

