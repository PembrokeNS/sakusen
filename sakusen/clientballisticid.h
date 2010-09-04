#ifndef LIBSAKUSEN__CLIENTBALLISTICID_H
#define LIBSAKUSEN__CLIENTBALLISTICID_H

#include <sakusen/idbase.h>

namespace sakusen {

/** \brief Id for a particular player's perspective of a ballictic.
 *
 * Each player that can see a Ballistic will give it an id.  These are
 * independent of one another; there is no overall id for a Ballistic, and if
 * no one can see it then it has no id at all. */
class ClientBallisticId : public IdBase<uint32, ClientBallisticId> {
};

}

#endif // LIBSAKUSEN__CLIENTBALLISTICID_H

