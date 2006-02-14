#ifndef UNIVERSEBRANCH_H
#define UNIVERSEBRANCH_H

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class UniverseBranch : public Branch {
  private:
    UniverseBranch();
  public:
    UniverseBranch(const Branch* parent, Server* server);
};

}}

#endif // UNIVERSEBRANCH_H

