#ifndef FUSEKI__SETTINGSTREE__SERVERBRANCH_H
#define FUSEKI__SETTINGSTREE__SERVERBRANCH_H

#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class ServerBranch : public Branch {
  public:
    typedef boost::shared_ptr<ServerBranch> Ptr;
    typedef boost::shared_ptr<const ServerBranch> ConstPtr;

    ServerBranch(Branch* parent, Server* server);
};

}}

#endif // FUSEKI__SETTINGSTREE__SERVERBRANCH_H

