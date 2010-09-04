#ifndef LIBSAKUSEN_RESOURCES__FILEIOEXN_H
#define LIBSAKUSEN_RESOURCES__FILEIOEXN_H

#include <sakusen/resources/libsakusen-resources-global.h>

#include <sakusen/exceptions.h>
#include <sakusen/comms/errorutils.h>

namespace sakusen {
namespace resources {

class LIBSAKUSEN_RESOURCES_API FileIOExn : public Exn
{
  private:
    FileIOExn(const String& message, int) :
      Exn(message)
    {}
  public:
    static FileIOExn customMessage(const String& message) {
      return FileIOExn(message, 0);
    }

    FileIOExn(const String& function) :
      Exn("IO error in "+function+": "+sakusen::comms::errorUtils_errorMessage(errno))
    {}
    virtual ~FileIOExn() throw() {}
};

}}

#endif // LIBSAKUSEN_RESOURCES__FILEIOEXN_H
