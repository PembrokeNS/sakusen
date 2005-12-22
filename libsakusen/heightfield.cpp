#include "heightfield.h"

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

