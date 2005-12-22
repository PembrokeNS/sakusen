#ifndef PLAYERSSETTINGSTREEBRANCH_H
#define PLAYERSSETTINGSTREEBRANCH_H

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
};

}}

#endif // PLAYERSSETTINGSTREEBRANCH_H

