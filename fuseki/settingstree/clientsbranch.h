#ifndef FUSEKI__SETTINGSTREE__CLIENTSBRANCH_H
#define FUSEKI__SETTINGSTREE__CLIENTSBRANCH_H

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class ClientsBranch : public Branch {
  public:
    typedef boost::shared_ptr<ClientsBranch> Ptr;

    ClientsBranch(Branch* parent, Server* server);
  public:
    void addClient(sakusen::ClientId id);
    void removeClient(sakusen::ClientId id);
};

}}

#endif // FUSEKI__SETTINGSTREE__CLIENTSBRANCH_H

