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
  type(orderType_move),
  data(d.newCopy())
{
}

Order::~Order()
{
  if (data != NULL) {
    /* Debugf(("[Order::~Order()] this=%x, data=%x",
          (unsigned int) this, (unsigned int) data)); */
    delete data;
    data = NULL;
  }
}

Order& Order::operator=(const Order& copy)
{
  type = copy.type;
  data = ( copy.data ? copy.data->newCopy() : NULL );
  return *this;
}

void Order::store(OArchive& out) const
{
  out << uint8(type);
  if (data != NULL) {
    data->store(out);
  }
}

Order Order::load(IArchive& in)
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
    default:
      Debug("Unknown OrderType: " << type);
      return Order();
  }
}

