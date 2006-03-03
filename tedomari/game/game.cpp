#include "game.h"

#include "resourceinterface-methods.h"

using namespace std;

using namespace sakusen;
using namespace sakusen::comms;
using namespace sakusen::client;
using namespace tedomari::game;

Game::Game(ResourceInterface* rI) :
  resourceInterface(rI),
  universe(NULL)
{
}

Game::~Game()
{
  if (sakusen::client::world != NULL) {
    stop();
  }
  delete universe;
  universe = NULL;
}

void Game::setUniverse(const String& name, const String& hash)
{
  ResourceSearchResult result;
  universe =
    resourceInterface->search<Universe>(name + "." + hash, NULL, &result);
  switch (result) {
    case resourceSearchResult_success:
      return;
    case resourceSearchResult_notFound:
    case resourceSearchResult_ambiguous:
    case resourceSearchResult_error:
      /* TODO: better error handling */
      Fatal("problem loading universe");
  }
}

void Game::start(const sakusen::comms::GameStartMessageData& data)
{
  assert(universe != NULL);
  new PartialWorld(
      universe,
      data.getTopology(),
      data.getTopRight(),
      data.getBottomLeft(),
      data.getGravity()
    );
}

void Game::pushUpdates(const UpdateMessageData& data) {
  dirty = true;
  while (data.getTime() > sakusen::client::world->getTimeNow()) {
    sakusen::client::world->endTick();
  }
  if (data.getTime() < sakusen::client::world->getTimeNow()) {
    Debug("Got updates in wrong order");
  }
  list<Update> updates(data.getUpdates());
  while (!updates.empty()) {
    sakusen::client::world->applyUpdate(updates.front());
    updates.pop_front();
  }
}

void Game::stop()
{
  delete sakusen::client::world;
}
