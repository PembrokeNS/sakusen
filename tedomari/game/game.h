#ifndef TEDOMARI__GAME__GAME_H
#define TEDOMARI__GAME__GAME_H

#include <queue>

#include "tedomari-global.h"
#include <sakusen/universe.h>
#include <sakusen/update.h>
#include <sakusen/resourceinterface.h>
#include <sakusen/ordermessage.h>
#include <sakusen/comms/messagedata.h>
#include <sakusen/client/partialworld.h>

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
    /** \brief Standard constructor
     *
     * \param resourceInterface The ResourceInterface that the Game should use
     * to load required game data. */
    Game(const sakusen::ResourceInterface::Ptr& resourceInterface);
    ~Game();
  private:
    sakusen::ResourceInterface::Ptr resourceInterface; /* not owned by this */
    ServerInterface* serverInterface; /* not owned by this */
    sakusen::Universe::Ptr universe; /* owned by this */

    uint64 availableEnergy;
    uint64 availableMetal;

    /** \bug This doesn't belong here */
    uint32 dexPerPixel;
    
    bool dirty;
  public:
    uint64 getAvailableEnergy() const { return availableEnergy; }
    uint64 getAvailableMetal() const { return availableMetal; }

    uint32 getDexPerPixel() const { return dexPerPixel; }

    /** \brief Checks for changes to game state
     *
     * \return true iff there have been changes to the game state which have
     * not yet propogated to the UI */
    bool isDirty() const { return dirty; }
    /** \brief Clears dirty flag
     *
     * This method called by the UI to indicate that it has updated itself to
     * the current game state. */
    void clearDirty() { dirty = false; }
    /** Returns true iff the game is underway */
    inline bool isStarted() const { return sakusen::client::world != NULL; }
    
    void setUniverse(const String& path, const String& hash);
    /** \brief Starts the game
     *
     * Creates world and starts the game.  A previous call to setUniverse
     * should have been made successfully.
     *
     * \param data Data from server describing initial game state.
     * \param serverInterface ServerInterface to be used to communicate with
     * server. */
    void start(
        const sakusen::comms::GameStartMessageData& data,
        ServerInterface* serverInterface
      );
    void pushUpdates(const sakusen::comms::UpdateMessageData& data);
    void order(const sakusen::OrderMessage&);
    void stop();
};

}}

#endif // TEDOMARI__GAME__GAME_H

