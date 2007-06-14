#ifndef CLIENTBRANCH_H
#define CLIENTBRANCH_H

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

#endif // CLIENTBRANCH_H

