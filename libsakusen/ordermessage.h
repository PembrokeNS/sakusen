#ifndef ORDERMESSAGE_H
#define ORDERMESSAGE_H

#include "libsakusen-global.h"

#include "order.h"

namespace sakusen {

class LIBSAKUSEN_API OrderMessage {
  public:
    OrderMessage();
    OrderMessage(UnitId orderee, const Order& order);
    ~OrderMessage() {}
  private:
    UnitId orderee;
    Order order;
  public:
    /** \name accessors */
    //@{
    UnitId getOrderee(void) const { return orderee; }
    const Order& getOrder(void) const { return order; }
    //@}

    void store(OArchive&) const;
    static OrderMessage load(IArchive&, const DeserializationContext&);
};

}

#endif // ORDERMESSAGE_H

