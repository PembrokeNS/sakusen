#ifndef GAMEBRANCH_H
#define GAMEBRANCH_H

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class GameBranch : public Branch {
  public:
    GameBranch(Branch* parent, Server* server);
};

}}

#endif // GAMEBRANCH_H

