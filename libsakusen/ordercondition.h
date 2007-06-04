#ifndef ORDERCONDITION_H
#define ORDERCONDITION_H

/* Note that current serialization implementation limits us to 256 enumeration
 * values (see [io]archive.cpp) */

namespace sakusen {

/** \brief Enumeration of conditions under which orders are to be executed
 * 
 * A condition is given along with each order to the unit which is to perform
 * the order.  A client which doesn't which to take advantage of this feature
 * should always use orderCondition_incidental */
enum OrderCondition {
  /** Order should be executed now, and should be the order which is considered
   * to have either succeeded or failed for the purposes of other conditions */
  orderCondition_now,
  /** Order should be executed now, but there should be no change to the order
   * which is considered to have either succeeded or failed for the purposes of
   * other conditions */
  orderCondition_incidental,
  /** Order should be executed when currently pending order succeeds */
  orderCondition_lastOrderSuccess,
  /** Order should be executed when currently pending order fails */
  orderCondition_lastOrderFailure,
  /** Order should be executed when unit sights an enemy which was not
   * previously visible */
  orderCondition_enemyUnitSighted,
  /** Dummy condition which must be the last */
  orderCondition_max
};

}

#endif // ORDERCONDITION_H

