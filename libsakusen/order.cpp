#include "order.h"

using namespace sakusen;

Order::Order() :
  type(orderType_none),
  data(NULL)
{
}

Order::Order(const Order& copy) :
  type(copy.type),
  data( copy.data ? copy.data->newCopy() : NULL )
{
}

Order::Order(const OrderData& d) :
  type(d.getType()),
  data(d.newCopy())
{
}

Order::~Order()
{
  /* Debugf(("[Order::~Order()] this=%x, data=%x",
        (unsigned int) this, (unsigned int) data)); */
  delete data;
  data = NULL;
}

Order& Order::operator=(const Order& copy)
{
  type = copy.type;
  data = ( copy.data ? copy.data->newCopy() : NULL );
  return *this;
}

void Order::store(OArchive& out) const
{
  out.insertEnum(type);
  if (data != NULL) {
    data->store(out);
  }
}

Order Order::load(IArchive& in, const PlayerID* player)
{
  OrderType type;
  in.extractEnum(type);
  
  switch (type) {
    case orderType_none:
      return Order();
    case orderType_move:
      return Order(MoveOrderData(in));
    case orderType_setVelocity:
      return Order(SetVelocityOrderData(in));
    case orderType_targetPoint:
      return Order(TargetPointOrderData(in));
    case orderType_targetSensorReturns:
      return Order(TargetSensorReturnsOrderData(in, player));
    default:
      Debug("Unknown OrderType: " << type);
      return Order();
  }
}

