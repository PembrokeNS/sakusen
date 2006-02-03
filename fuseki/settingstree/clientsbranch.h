#ifndef CLIENTSBRANCH_H
#define CLIENTSBRANCH_H

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class ClientsBranch : public Branch {
  private:
    ClientsBranch();
    ClientsBranch(const ClientsBranch&);
  public:
    ClientsBranch(const Branch* parent, Server* server);
    ~ClientsBranch();
  public:
    void addClient(sakusen::comms::ClientID id);
    void removeClient(sakusen::comms::ClientID id);
};

}}

#endif // CLIENTSBRANCH_H

