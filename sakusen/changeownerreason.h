#ifndef LIBSAKUSEN__CHANGEOWNERREASON_H
#define LIBSAKUSEN__CHANGEOWNERREASON_H

namespace sakusen {

/** Describes a reason for a Unit's owner being changed, primarily so the client
 * can notify the user appropriately.
 */
enum changeOwnerReason {
  changeOwnerReason_created, /**< newly created unit */
  changeOwnerReason_destroyed, /**< finally destroyed unit */
  changeOwnerReason_given, /**< voluntarily, the owner already knows it is going
                              to happen */
  changeOwnerReason_captured, /**< permamently */
  changeOwnerReason_confused, /**< temporarily taken control of by another
                                 player or by neutral */
  changeOwnerReason_reverted /**< reverted to original owner's control after
                                being confused */
};

}

#endif // LIBSAKUSEN__CHANGEOWNERREASON_H

