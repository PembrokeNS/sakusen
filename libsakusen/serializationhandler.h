#ifndef LIBSAKUSEN__SERIALIZATIONHANDLER_H
#define LIBSAKUSEN__SERIALIZATIONHANDLER_H

#include "ref.h"
#include "playerid.h"

namespace sakusen {

/** \brief Helper class for (de)serialization of Refs
 *
 * Each class T for which a Ref<T> might be (de)serialized must specialize
 * SerializationHandler<T> and provide extract and insert methods to perform
 * such serialization.  See any of the existing examples for the method
 * signatures and an idea of how this works. */
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

