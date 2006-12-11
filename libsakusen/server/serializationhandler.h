#ifndef LIBSAKUSEN_SERVER__SERIALIZATIONHANDLER_H
#define LIBSAKUSEN_SERVER__SERIALIZATIONHANDLER_H

#include "ref.h"

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
  public:
    typedef void loadArgument;
};

/** \todo Write class */
template<>
class SerializationHandler<server::Ballistic>
{
  public:
    typedef void loadArgument;
};

/** \todo Write class */
template<>
class SerializationHandler<server::Beam>
{
  public:
    typedef void loadArgument;
};

/** \todo Write class */
template<>
class SerializationHandler<server::Effect>
{
  public:
    typedef void loadArgument;
};

/** This exists only to appease the template instantiator */
template<>
class SerializationHandler<server::DynamicSensorReturns>
{
  public:
    typedef void loadArgument;
};

}

#endif // LIBSAKUSEN_SERVER__SERIALIZATIONHANDLER_H

