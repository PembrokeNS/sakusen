#ifndef GAME_H
#define GAME_H

#include <queue>

#include "universe.h"
#include "update.h"
#include "resourceinterface.h"
#include "ordermessage.h"
#include "messagedata.h"
#include "partialworld.h"

namespace tedomari {

class ServerInterface;

namespace game {

/** \brief Handles maintainance of the game state for the client.
 *
 * This class manages the game state, including, for example loading the
 * Universe, initializing the PartialWorld, etc. */
class Game {
  private:
    Game();
    Game(const Game&);
  public:
    Game(sakusen::ResourceInterface* resourceInterface);
    ~Game();
  private:
    sakusen::ResourceInterface* resourceInterface; /* not owned by this */
    ServerInterface* serverInterface; /* not owned by this */
    sakusen::Universe* universe; /* owned by this */
    
    /** true iff there have been changes to the game state which have not yet
     * propogated to the UI */
    bool dirty;
  public:
    bool isDirty() const { return dirty; }
    void clearDirty() { dirty = false; }
    /** Returns true iff the game is underway */
    inline bool isStarted() const { return sakusen::client::world != NULL; }
    void pushUpdates(const sakusen::comms::UpdateMessageData& data);

    void setUniverse(const String& name, const String& hash);
    void start(
        const sakusen::comms::GameStartMessageData&,
        ServerInterface* serverInterface
      );
    void order(const sakusen::OrderMessage&);
    void stop();
};

}}

#endif // GAME_H

