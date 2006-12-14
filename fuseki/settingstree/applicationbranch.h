#ifndef APPLICATIONBRANCH_H
#define APPLICATIONBRANCH_H

#include "branch.h"

namespace fuseki {
namespace settingsTree {

class ApplicationBranch : public Branch {
  public:
    ApplicationBranch(
        const String& readers,
        const String& writers, /* groups with write permission to this node */
        const String& subWriters, /* groups with write permission to child
                                     nodes */
        Branch* parent,
        Server* server
      );
};

}}

#endif // APPLICATIONBRANCH_H

