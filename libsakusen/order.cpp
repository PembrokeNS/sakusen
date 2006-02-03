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

Order::Order(const MoveOrderData& d) :
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

