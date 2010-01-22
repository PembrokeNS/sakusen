#ifndef FUSEKI__SETTINGSTREE__CLIENTBRANCH_H
#define FUSEKI__SETTINGSTREE__CLIENTBRANCH_H

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class ClientBranch : public Branch {
  public:
    ClientBranch(
        sakusen::ClientId id,
        Branch* parent,
        Server* server
      );
};

}}

#endif // FUSEKI__SETTINGSTREE__CLIENTBRANCH_H

