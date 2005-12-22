#ifndef APPLICATIONBRANCH_H
#define APPLICATIONBRANCH_H

#include "branch.h"

namespace fuseki {
namespace settingsTree {

class ApplicationBranch : public Branch {
  private:
    ApplicationBranch();
    ApplicationBranch(const ApplicationBranch&);
  public:
    ApplicationBranch(
        const String& readers,
        const String& writers, /* groups with write permission to this node */
        const String& subWriters, /* groups with write permission to child
                                     nodes */
        const Branch* parent,
        Server* server
      );
    ~ApplicationBranch();
};

}}

#endif // APPLICATIONBRANCH_H

