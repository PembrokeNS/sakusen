#ifndef ORDERMESSAGE_H
#define ORDERMESSAGE_H

#include "libsakusen-global.h"

#include "ordercondition.h"
#include "order.h"

class LIBSAKUSEN_API OrderMessage {
  public:
    OrderMessage();
    OrderMessage(uint32 orderee, OrderCondition condition, const Order& order);
    ~OrderMessage() {}
  private:
    uint32 orderee;
    OrderCondition condition;
    Order order;
  public:
    PLACEHOLDER_OPERATORS(OrderMessage)
    /* accessors */
    uint32 getOrderee(void) const { return orderee; }
    const OrderCondition& getCondition(void) const { return condition; }
    const Order& getOrder(void) const { return order; }
};

#ifdef NEED_TEMPLATE_INSTANTIATION
#include <queue>
#pragma warning (disable: 4231)
EXPORT_QUEUE(OrderMessage)
#endif

#endif // ORDERMESSAGE_H

