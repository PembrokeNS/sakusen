#ifndef GAMEBRANCH_H
#define GAMEBRANCH_H

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class GameBranch : public Branch {
  private:
    GameBranch();
    GameBranch(const GameBranch&);
  public:
    GameBranch(const Branch* parent, Server* server);
    ~GameBranch() {}
};

}}

#endif // GAMEBRANCH_H

