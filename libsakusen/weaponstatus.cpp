#include "weaponstatus.h"

using namespace sakusen;

void WeaponStatus::store(OArchive& /*archive*/) const
{
}

WeaponStatus WeaponStatus::load(IArchive& /*archive*/)
{
  return WeaponStatus();
}

