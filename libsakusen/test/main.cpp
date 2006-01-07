/* This is intended to be a quick and superficial test of libsakusen, although
 * with luck it will become less superficial over time */



#include <iostream>
#include <time.h>
#include <cerrno>

#include "libsakusen-global.h"
#include "world.h"
#include "planemap.h"
#include "debuggingclient.h"
#include "patrollerclient.h"

#define NANO 1000000000 /* Nanoseconds in a second */

using namespace std;

void doLoadTest(ostream& output)
{
  uint32 numFrames = 1000;
#ifndef _MSC_VER
  struct timespec startTime;
  struct timespec endTime;
  /* get time before test */
  if (clock_gettime(CLOCK_REALTIME, &startTime)) {
    Debug("Clock error");
    cerr << "errno=" << errno << endl;
    exit(1);
  }
#endif
  /* do test */
  world->advanceGameState(numFrames);
#ifndef _MSC_VER
  /* get time after test */
  if (clock_gettime(CLOCK_REALTIME, &endTime)) {
    Debug("Clock error");
    cerr << "errno=" << errno << endl;
    exit(1);
  }
  /* compute framerate */
  uint64 timeTaken = endTime.tv_sec - startTime.tv_sec;
  timeTaken *= NANO;
  timeTaken += endTime.tv_nsec - startTime.tv_nsec;
  timeTaken /= numFrames;
  output << "Test complete: ";
  output << endl;
  if (timeTaken) {
    output << "Framerate=" << NANO/timeTaken << "fps" << endl;
  } else {
    output << "Nanoseconds per frame=" << timeTaken << endl;
  }
#else
  cout << "Test complete." << endl;
#endif
}

int main(/* int argc, char** argv */)
{
  /* First test is with an empty map of size zero */
  
  cout << "Performing test with blank map..." << endl;
  /* Create a debugging client */
  DebuggingClient client(cout);
  vector<Client*> clients;
  clients.push_back(&client);

  /* For our toy example we create two players: the neutral player with no
   * client, and a "real" player with a debugging client */
  Player neutralPlayer = Player();
  Player realPlayer = Player();
  realPlayer.attachClient(&client);
  vector<Player> players;
  players.push_back(neutralPlayer);
  players.push_back(realPlayer);

  /* Create the universe */
  vector<UnitType> unitTypes;
  UnitType commanderType =
    UnitType(
        "commander" /* name */,
        100 /* maxHitPoints */,
        100 /* energyCost */,
        100 /* metalCost */,
        10 /* mass */,
        Point<uint32>(10,10,10) /* size */,
        Point<uint16>(10,10,10) /* maxAcceleration */,
        Point<uint16>(10,10,10) /* maxMinusAcceleration */,
        Point<sint16>(10,10,10) /* maxSpeed */,
        Point<sint16>(10,10,10) /* maxMinusSpeed */,
        10 /* maxHorizTurnSpeed */,
        5 /* maxVertTurnSpeed */,
        Visibility(),
        Sensors(),
        false /* fixed */,
        true /* ground */,
        false /* surface */,
        false /* air */,
        true /* seabed */,
        std::list<WeaponTypeID>(),
        NULL /* corpseUnitType */
      );
  unitTypes.push_back(commanderType);
  
  Universe universe("universe", unitTypes);

  /* Create the map */
  Heightfield heightfield = Heightfield();
  vector<Unit> neutralPlayersUnits;
  vector<Unit> realPlayersUnits;
  vector< vector<Unit> > playersUnits;
  playersUnits.push_back(neutralPlayersUnits);
  playersUnits.push_back(realPlayersUnits);
  MapPlayMode playMode = MapPlayMode(2, 2, playersUnits);
  vector<MapPlayMode> playModes;
  playModes.push_back(playMode);
  Map* map = new PlaneMap(
      &universe, "Toy map", Point<uint32>(), heightfield,
      10 /* gravity */, playModes
    );

  /* Create the world */
  World* w = new World(map, 0 /* mode */, players);
    
  doLoadTest(cout);
  world = NULL;
  delete w;
  w = NULL;
  delete map;

  /* Now another test, this time with a unit */
  cout << "Performing test with one unit..." << endl;
  
  /* Create the map */
  heightfield = Heightfield();
  realPlayersUnits.push_back(
      Unit(
        universe.getUnitTypeId(0),
        Point<sint32>(1,1,1),
        Orientation(),
        Point<sint16>(),
        &universe
      )
    );
  playersUnits.clear();
  playersUnits.push_back(neutralPlayersUnits);
  playersUnits.push_back(realPlayersUnits);
  playMode = MapPlayMode(2, 2, playersUnits);
  playModes.clear();
  playModes.push_back(playMode);
  map = new PlaneMap(
      &universe, "Toy map", Point<uint32>(10000,10000,10000),
      heightfield, 10 /* gravity */, playModes
    );

  /* Create the world */
  w = new World(map, 0 /* mode */, players);
  
  doLoadTest(cout);
  world = NULL;
  delete w;
  w = NULL;

  /* Do a test with a unit patrolling */
  cout << "Performing test with a patrolling unit..." << endl;
  
  PatrollerClient patrollerClient(Point<sint32>(2000, 0, 0));
  players.back().attachClient(&patrollerClient);
  
  w = new World(map, 0 /* mode */, players);
  doLoadTest(cout);
  world = NULL;
  delete w;
  w = NULL;
  delete map;

  /* Done */
  cout << "All tests completed successfully." << endl;
  return 0;
}

