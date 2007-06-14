#ifndef PLAYERBRANCH_H
#define PLAYERBRANCH_H

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

#endif // PLAYERBRANCH_H


