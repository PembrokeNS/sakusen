#ifndef LIBSAKUSEN_SERVER__SERIALIZATIONHANDLER_H
#define LIBSAKUSEN_SERVER__SERIALIZATIONHANDLER_H

#include <sakusen/ref.h>

namespace sakusen {
namespace server {

class LayeredUnit;
class Ballistic;
class Beam;
class Effect;
class DynamicSensorReturns;

} /* back into namespace sakusen */

/** \todo Write class */
template<>
class SerializationHandler<server::LayeredUnit>
{
};

/** \todo Write class */
template<>
class SerializationHandler<server::Ballistic>
{
};

/** \todo Write class */
template<>
class SerializationHandler<server::Beam>
{
};

/** \todo Write class */
template<>
class SerializationHandler<server::Effect>
{
};

}

#endif // LIBSAKUSEN_SERVER__SERIALIZATIONHANDLER_H

