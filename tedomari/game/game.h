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
    std::queue<sakusen::Update> updateQueue;
  public:
    inline bool isStarted() const { return sakusen::client::world != NULL; }
    inline void pushUpdate(const sakusen::Update& u) { updateQueue.push(u); }

    void setUniverse(const String& name, const String& hash);
    void start(const sakusen::comms::GameStartMessageData&);
    void stop();
};

}}

#endif // GAME_H

