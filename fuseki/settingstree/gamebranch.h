#ifndef FUSEKI__SETTINGSTREE__GAMEBRANCH_H
#define FUSEKI__SETTINGSTREE__GAMEBRANCH_H

#include "global.h"

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class GameBranch : public Branch {
  public:
    GameBranch(Branch* parent, Server* server);
};

}}

#endif // FUSEKI__SETTINGSTREE__GAMEBRANCH_H

