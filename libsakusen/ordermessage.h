#ifndef ORDERMESSAGE_H
#define ORDERMESSAGE_H

#include "libsakusen-global.h"

#include "order.h"

namespace sakusen {

class LIBSAKUSEN_API OrderMessage {
  public:
    OrderMessage();
    OrderMessage(UnitID orderee, const Order& order);
    ~OrderMessage() {}
  private:
    UnitID orderee;
    Order order;
  public:
    /** \name accessors */
    //@{
    UnitID getOrderee(void) const { return orderee; }
    const Order& getOrder(void) const { return order; }
    //@}

    typedef PlayerID loadArgument;
    void store(OArchive&) const;
    static OrderMessage load(IArchive&, const loadArgument*);
};

}

#endif // ORDERMESSAGE_H

