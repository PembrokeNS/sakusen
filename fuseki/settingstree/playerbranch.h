#ifndef FUSEKI__SETTINGSTREE__PLAYERBRANCH_H
#define FUSEKI__SETTINGSTREE__PLAYERBRANCH_H

#include "settingstree/branch.h"

#include "playerid.h"
#include "playertemplate.h"

namespace fuseki {
namespace settingsTree {

class PlayerBranch : public Branch {
  public:
    PlayerBranch(
        sakusen::PlayerId id,
        Branch* parent,
        Server* server,
        const sakusen::PlayerTemplate& t
      );
};

}}

#endif // FUSEKI__SETTINGSTREE__PLAYERBRANCH_H


