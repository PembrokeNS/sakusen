#ifndef SERVERBRANCH_H
#define SERVERBRANCH_H

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class ServerBranch : public Branch {
  public:
    ServerBranch(Branch* parent, Server* server);
};

}}

#endif // SERVERBRANCH_H

