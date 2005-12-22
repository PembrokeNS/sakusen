#ifndef ORDERCONDITION_H
#define ORDERCONDITION_H

enum OrderCondition {
  orderCondition_now,
  orderCondition_lastOrderSuccess,
  orderCondition_lastOrderFailure,
  orderCondition_enemyUnitSighted,
  orderCondition_max /* Dummy condition which must be the last */
};

#endif // ORDERCONDITION_H

