#ifndef CLIENTBALLISTICID_H
#define CLIENTBALLISTICID_H

namespace sakusen {

/** \brief Id for a particular player's perspective of a ballictic.
 *
 * Each player that can see a Ballistic will give it an id.  These are
 * independent of one another; there is no overall id for a Ballistic, and if
 * no one can see it then it has no id at all. */
typedef uint32 ClientBallisticId;

}

#endif // CLIENTBALLISTICID_H

