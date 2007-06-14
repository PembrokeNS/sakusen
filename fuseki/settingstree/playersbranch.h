#ifndef PLAYERSSETTINGSTREEBRANCH_H
#define PLAYERSSETTINGSTREEBRANCH_H

#include "playerid.h"
#include "playertemplate.h"
#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class PlayersBranch : public Branch {
  public:
    typedef boost::shared_ptr<PlayersBranch> Ptr;
  public:
    PlayersBranch(Branch* parent, Server* server);

    void addPlayer(sakusen::PlayerId id, const sakusen::PlayerTemplate& t);
    void removePlayer(sakusen::PlayerId id);
};

}}

#endif // PLAYERSSETTINGSTREEBRANCH_H

