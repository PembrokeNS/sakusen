#ifndef CLIENTBALLISTICID_H
#define CLIENTBALLISTICID_H

namespace sakusen {

/** \brief ID for a particular player's perspective of a ballictic.
 *
 * Each player that can see a Ballistic will give it an ID.  These are
 * independent of one another; there is no overall ID for a Ballistic, and if
 * no one can see it then it has no ID at alli. */
typedef uint32 ClientBallisticID;

}

#endif // CLIENTBALLISTICID_H

