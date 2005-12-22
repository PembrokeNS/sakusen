#ifndef CLIENTBRANCH_H
#define CLIENTBRANCH_H

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class ClientBranch : public Branch {
  private:
    ClientBranch();
    ClientBranch(const ClientBranch&);
  public:
    ClientBranch(ClientID id, const Branch* parent, Server* server);
    ~ClientBranch() {}
};

}}

#endif // CLIENTBRANCH_H

