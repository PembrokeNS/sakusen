#ifndef SERVERBRANCH_H
#define SERVERBRANCH_H

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class ServerBranch : public Branch {
  private:
    ServerBranch();
    ServerBranch(const ServerBranch&);
  public:
    ServerBranch(const Branch* parent, Server* server);
    ~ServerBranch();
};

}}

#endif // SERVERBRANCH_H

