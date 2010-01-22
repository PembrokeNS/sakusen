#ifndef FUSEKI__SETTINGSTREE__UNIVERSEBRANCH_H
#define FUSEKI__SETTINGSTREE__UNIVERSEBRANCH_H

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class UniverseBranch : public Branch {
  public:
    UniverseBranch(Branch* parent, Server* server);
};

}}

#endif // FUSEKI__SETTINGSTREE__UNIVERSEBRANCH_H

