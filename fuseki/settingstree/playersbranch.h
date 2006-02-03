#ifndef PLAYERSSETTINGSTREEBRANCH_H
#define PLAYERSSETTINGSTREEBRANCH_H

#include "playerid.h"
#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class PlayersBranch : public Branch {
  private:
    PlayersBranch();
    PlayersBranch(const PlayersBranch&);
  public:
    PlayersBranch(const Branch* parent, Server* server);
    ~PlayersBranch();

    void addPlayer(sakusen::PlayerID id, bool special, bool fixed);
    void removePlayer(sakusen::PlayerID id);
};

}}

#endif // PLAYERSSETTINGSTREEBRANCH_H

