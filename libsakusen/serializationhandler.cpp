#include "serializationhandler.h"

#include "world.h"
#include "icompleteunit.h"

namespace sakusen {

Ref<ICompleteUnit> SerializationHandler<ICompleteUnit>::extract(
    IArchive& archive,
    const PlayerID* player
  ) const {
  uint32 unitID;
  archive >> unitID;
  return world->getICompleteUnit(*player, unitID);
}

void SerializationHandler<ICompleteUnit>::insert(
    OArchive& archive,
    const Ref<const ICompleteUnit>& unit
  ) const {
  archive << unit->getId();
}

}

