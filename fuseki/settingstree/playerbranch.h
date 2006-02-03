#ifndef PLAYERBRANCH_H
#define PLAYERBRANCH_H

#include "settingstree/branch.h"

#include "playerid.h"

namespace fuseki {
namespace settingsTree {

class PlayerBranch : public Branch {
  private:
    PlayerBranch();
    PlayerBranch(const PlayerBranch&);
  public:
    PlayerBranch(
        sakusen::PlayerID id,
        const Branch* parent,
        Server* server,
        bool special,
        bool fixed
      );
    ~PlayerBranch() {}
};

}}

#endif // PLAYERBRANCH_H


