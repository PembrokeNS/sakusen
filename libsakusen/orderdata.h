#ifndef ORDERDATA_H
#define ORDERDATA_H

#include "libsakusen-global.h"

class LIBSAKUSEN_API OrderData {
  /* abstract class - all constructors must be protected */
  protected:
    OrderData();
    OrderData(const OrderData& copy);
  public:
    virtual ~OrderData() {};
    virtual OrderData* newCopy(void) const = 0;
      /* Allocates a new copy of this object - caller responsible for freeing
       * memory */
};

#endif // ORDERDATA_H

