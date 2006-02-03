#include "heightfield.h"

using namespace sakusen;

Heightfield::Heightfield()
{
}

void Heightfield::store(OArchive&) const
{
}

Heightfield Heightfield::load(IArchive&)
{
  return Heightfield();
}

