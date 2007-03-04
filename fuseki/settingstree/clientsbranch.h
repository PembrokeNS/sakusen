#ifndef CLIENTSBRANCH_H
#define CLIENTSBRANCH_H

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class ClientsBranch : public Branch {
  public:
    typedef boost::shared_ptr<ClientsBranch> Ptr;

    ClientsBranch(Branch* parent, Server* server);
  public:
    void addClient(sakusen::ClientID id);
    void removeClient(sakusen::ClientID id);
};

}}

#endif // CLIENTSBRANCH_H

