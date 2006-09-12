#ifndef ORDERMESSAGE_H
#define ORDERMESSAGE_H

#include "libsakusen-global.h"

#include "ordercondition.h"
#include "order.h"

namespace sakusen {

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
    /* accessors */
    uint32 getOrderee(void) const { return orderee; }
    const OrderCondition& getCondition(void) const { return condition; }
    const Order& getOrder(void) const { return order; }

    typedef PlayerID loadArgument;
    void store(OArchive&) const;
    static OrderMessage load(IArchive&, const loadArgument*);
};

}

#endif // ORDERMESSAGE_H

