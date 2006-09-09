#ifndef ORDERCONDITION_H
#define ORDERCONDITION_H

/* Note that current serialization implementation limits us to 256 enumeration
 * values (see [io]archive.cpp) */

namespace sakusen {

enum OrderCondition {
  orderCondition_now,
  orderCondition_incidental,
  orderCondition_lastOrderSuccess,
  orderCondition_lastOrderFailure,
  orderCondition_enemyUnitSighted,
  orderCondition_max /* Dummy condition which must be the last */
};

}

#endif // ORDERCONDITION_H

