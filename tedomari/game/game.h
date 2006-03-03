#ifndef GAME_H
#define GAME_H

#include "universe.h"
#include "update.h"
#include "resourceinterface.h"
#include "partialworld.h"
#include "messagedata.h"

#include <queue>

namespace tedomari {
namespace game {

class Game {
  private:
    Game();
    Game(const Game&);
  public:
    Game(sakusen::ResourceInterface* resourceInterface);
    ~Game();
  private:
    sakusen::ResourceInterface* resourceInterface; /* not owned by this */
    sakusen::Universe* universe; /* owned by this */
  public:
    bool dirty;
    
    inline bool isStarted() const { return sakusen::client::world != NULL; }
    void pushUpdates(const sakusen::comms::UpdateMessageData& data);

    void setUniverse(const String& name, const String& hash);
    void start(const sakusen::comms::GameStartMessageData&);
    void stop();
};

}}

#endif // GAME_H

