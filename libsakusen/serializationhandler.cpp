#include "serializationhandler.h"

#include "world.h"
#include "icompleteunit.h"

namespace sakusen {

Ref<ICompleteUnit> SerializationHandler<ICompleteUnit>::extract(
    IArchive& archive,
    const DeserializationContext& context
  ) const {
  UnitId unitId;
  archive >> unitId;
  return world->getICompleteUnit(context.getPlayerId(), unitId);
}

void SerializationHandler<ICompleteUnit>::insert(
    OArchive& archive,
    const Ref<const ICompleteUnit>& unit
  ) const {
  archive << unit->getId();
}

}

