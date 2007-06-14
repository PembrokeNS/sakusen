#ifndef DESERIALIZATIONCONTEXT_H
#define DESERIALIZATIONCONTEXT_H

#include "resourceinterface.h"
#include "playerid.h"

namespace sakusen {

class Universe;

/** \brief Context to provide hints and facility for deserialization.
 *
 * There are essentially two sorts of deserialization: that of resources and
 * that of messages.  Naively, these can by thought of as things from disk and
 * things from the network respectively, but in practive messages can come from
 * disk (e.g. replays) and resources from the network (e.g. during game init).
 *
 * When deserializing resources, the DeserializationContext provides a
 * ResourceInterface which can be used to load associated data (e.g. looking up
 * symbols in a module or loading a heightfield PNG).
 *
 * When deserializing messages, the DeserializationContext may provide a
 * PlayerID for the player from whose client the message was received.  This
 * can be used, for example, to find units based only on their client-side ID.
 * The message itself cannot be used to store the PlayerID, because that would
 * allow one player to masquerade as another.  When not set, this has the value
 * PlayerID(-1).
 *
 * In either case, the DeserializationContext may provide a Universe, if one
 * has yet been loaded and there is reason to believe it cannot be obtained by
 * querying world.  This can be used, for example, to look up UnitTypes
 * from their names.
 */
class DeserializationContext {
  public:
    explicit DeserializationContext(
        const ResourceInterface::Ptr& rI,
        boost::shared_ptr<const Universe> u=boost::shared_ptr<const Universe>()
      ) :
      resourceInterface(rI),
      universe(u),
      playerId(-1)
    {}
    explicit DeserializationContext(
        PlayerID pI = PlayerID(-1),
        boost::shared_ptr<const Universe> u=boost::shared_ptr<const Universe>()
      ) :
      resourceInterface(),
      universe(u),
      playerId(pI)
    {}
  private:
    ResourceInterface::Ptr resourceInterface;
    boost::shared_ptr<const Universe> universe;
    PlayerID playerId;
  public:
    ResourceInterface::Ptr getResourceInterface() const {
      return resourceInterface;
    }
    boost::shared_ptr<const Universe> getUniverse() const { return universe; }
    PlayerID getPlayerId() const { return playerId; }
};

}

#endif // DESERIALIZATIONCONTEXT_H

