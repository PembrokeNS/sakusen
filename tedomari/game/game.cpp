#include "game.h"

#include "resourceinterface-methods.h"
#include "messagedata.h"
#include "serverinterface.h"
#include "game/smartunitfactory.h"
#include "game/smartsensorreturnsfactory.h"
#include "heightfield-methods.h"

using namespace std;

using namespace sakusen;
using namespace sakusen::comms;
using namespace sakusen::client;
using namespace tedomari;
using namespace tedomari::game;

Game::Game(const sakusen::ResourceInterface::Ptr& rI) :
  resourceInterface(rI),
  serverInterface(NULL),
  universe(),
  availableEnergy(0),
  availableMetal(0),
  dexPerPixel(100),
  dirty(false)
{
}

/** \brief Destructor
 *
 * The destructor will stop the game and destroy world if appropriate.  Further
 * behavior is not excluded. */
Game::~Game()
{
  if (sakusen::client::world != NULL) {
    stop();
  }
}

/** \brief Loads the specified Universe and stores it for later use.
 *
 * \param path Salusen path to the Universe.
 * \param hash Hash of the Universe (as hex string). */
void Game::setUniverse(const String& path, const String& hash)
{
  ResourceSearchResult result;
  boost::tie(universe, result, boost::tuples::ignore) =
    resourceInterface->search<Universe>(path, hash);
  switch (result) {
    case resourceSearchResult_success:
      return;
    case resourceSearchResult_notFound:
    case resourceSearchResult_ambiguous:
    case resourceSearchResult_error:
    case resourceSearchResult_notSupported:
      /** \todo Better error handling */
      SAKUSEN_FATAL("problem loading universe '"<<path<<"': "<<result);
    default:
      SAKUSEN_FATAL("unexpected enum value: " << result);
  }
}

void Game::start(
    const sakusen::comms::GameStartMessageData& data,
    ServerInterface* sI
  )
{
  assert(universe != NULL);
  serverInterface = sI;
  new PartialWorld(
      universe,
      new SmartUnitFactory(),
      new SmartSensorReturnsFactory(),
      data.getPlayerId(),
      data.getTopology(),
      data.getTopRight(),
      data.getBottomLeft(),
      data.getGravity(),
      data.getHeightfield()
    );
  dexPerPixel = data.getDexPerPixel();
}

/** \brief Applies updates to game state
 *
 * Processes the given updates and alters the game state appropriately.  Also
 * sets dirty flag. */
void Game::pushUpdates(const UpdateMessageData& data) {
  dirty = true;
  availableEnergy = data.getEnergy();
  availableMetal = data.getMetal();
  while (data.getTime() > sakusen::client::world->getTimeNow()) {
    sakusen::client::world->endTick();
  }
  if (data.getTime() < sakusen::client::world->getTimeNow()) {
    SAKUSEN_DEBUG("Got updates in wrong order");
  }
  list<Update> updates(data.getUpdates());
  while (!updates.empty()) {
    SAKUSEN_QDEBUG(" got " << updates.front());
    sakusen::client::world->applyUpdate(updates.front());
    updates.pop_front();
  }
}

/** \brief Sends given order to server at once
 *
 * \param o The order to send */
void Game::order(const OrderMessage& o)
{
  serverInterface->send(new OrderMessageData(o));
}

/** \brief Stops the game and destroys the world */
void Game::stop()
{
  delete sakusen::client::world;
  serverInterface = NULL;
}

