#ifndef WEAPONSTATUS_H
#define WEAPONSTATUS_H

#include "iarchive.h"
#include "oarchive.h"

namespace sakusen {

class WeaponStatus {
  public:
    void store(OArchive&) const;
    static WeaponStatus load(IArchive&);
};

}

#endif // WEAPONSTATUS_H

