#ifndef UPDATETYPE_H
#define UPDATETYPE_H

#include <iosfwd>

namespace sakusen {

/* The various messages sent from server to client to inform of an update to
 * the game state.  What arguments each takes is given in a comment after it.
 * Note that current serialization implementation limits us to 256 types. */

enum UpdateType {
  updateType_none, /* Dummy value because mistakes will most probably lead to
                      a value of zero */
  updateType_unitRemoved, /* UnitID, changeOwnerReason */
  updateType_unitAdded, /* Unit, changeOwnerReason */
  updateType_orderQueued, /* UnitID, Order, OrderCondition */
    /* Given order placed in the queue with the given condition */
  updateType_orderCompleted, /* UnitID, OrderCondition */
    /* Order finished with success or failure given by condition, but no new
     * order accepted to replace it */
  updateType_orderAccepted /* UnitID, OrderCondition */
    /* Order accepted from specified queue entry */
};

std::ostream& operator<<(std::ostream& output, UpdateType update);

}

#endif // UPDATETYPE_H

