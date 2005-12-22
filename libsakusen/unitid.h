#ifndef UNITID_H
#define UNITID_H

#include "playerid.h"

class Unit;

class UnitID {
  public:
  UnitID();
  ~UnitID() {}
  private:
  PlayerID player;
  uint16 unit;
  public:
  Unit* operator->(void);
};

#endif // UNITID_H

