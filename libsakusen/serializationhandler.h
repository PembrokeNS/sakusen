#ifndef LIBSAKUSEN__SERIALIZATIONHANDLER_H
#define LIBSAKUSEN__SERIALIZATIONHANDLER_H

#include "ref.h"
#include "playerid.h"

namespace sakusen {

template<typename T>
class SerializationHandler {
  public:
    typedef void loadArgument;
};

class ICompleteUnit;

template<>
class SerializationHandler<ICompleteUnit> {
  public:
    Ref<ICompleteUnit> extract(
        IArchive& archive,
        const PlayerID* player
      ) const;
    void insert(OArchive& archive, const Ref<const ICompleteUnit>& unit) const;
    typedef PlayerID loadArgument;
};

}

#endif // LIBSAKUSEN__SERIALIZATIONHANDLER_H

