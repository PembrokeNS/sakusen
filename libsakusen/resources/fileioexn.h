#ifndef FILEIOEXN_H
#define FILEIOEXN_H

#include "exceptions.h"
#include "errorutils.h"

namespace sakusen {
namespace resources {

class FileIOExn : public Exn
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

#endif // FILEIOEXN_H
