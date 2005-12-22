#ifndef HEIGHTFIELD_H
#define HEIGHTFIELD_H

#include "libsakusen-global.h"
#include "point.h"
#include "iarchive.h"
#include "oarchive.h"

class LIBSAKUSEN_API Heightfield {
  public:
    Heightfield();
    ~Heightfield() {}
  private:
  public:
    void store(OArchive&) const;
    static Heightfield load(IArchive&);
};

#endif // HEIGHTFIELD_H

