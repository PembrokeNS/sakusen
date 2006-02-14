#include "game.h"

#include "resourceinterface-methods.h"

using namespace sakusen;
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

void Game::stop()
{
  delete sakusen::client::world;
}
