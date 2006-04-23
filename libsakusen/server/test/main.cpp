/* This is intended to be a quick and superficial test of libsakusen, although
 * with luck it will become less superficial over time */

#include <iostream>
#include <time.h>
#include <cerrno>
#include "libsakusen-global.h"
#include "completeworld.h"
#include "planemap.h"
#include "point.h"
#include "sphereregiondata.h"
#include "region.h"
#include "debuggingclient.h"
#include "patrollerclient.h"


#define NANO 1000000000 /* Nanoseconds in a second */

using namespace std;

using namespace sakusen;
using namespace sakusen::server;
using namespace sakusen::server::test;

void doLoadTest(ostream& output)
{
  uint32 numTicks = 1000;
#ifndef _MSC_VER
  struct timespec startTime;
  struct timespec endTime;
  /* get time before test */
  if (clock_gettime(CLOCK_REALTIME, &startTime)) {
    Debug("Clock error");
    cerr << "errno=" << errno << endl;
    exit(1);
  }
#else
  clock_t startTime, endTime;
  startTime=clock();
#endif

  /* do test */
  sakusen::server::world->advanceGameState(numTicks);
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
  timeTaken /= numTicks;
  output << "Test complete: ";
  output << endl;
  if (timeTaken) {
    output << "Tickrate=" << NANO/timeTaken << "tps" << endl;
  } else {
    output << "Test was too fast to time (probably indicates a problem)" <<
      endl;
  }
#else
  endTime=clock();
  clock_t timeTaken = endTime - startTime;
  cout << "Test complete." << endl;
  if (0 != timeTaken) {
    cout << "Tickrate: "<<numTicks / timeTaken<<" t/ms "<<endl;
  } 
  else {
    cout<<"Test too fast to time."<<endl;
  }
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

  /* Create the universe */
  vector<UnitType> unitTypes;
  UnitType commanderType =
    UnitType(
        "commander" /* name */,
        UnitTypeData(
          100 /* maxHitPoints */,
          10 /* mass */,
          Point<uint32>(10,10,10) /* size */,
          Region<sint16>(SphereRegionData<sint16>(Point<sint16>(), 10))
            /* possibleAccelerations */,
          Region<sint16>(SphereRegionData<sint16>(Point<sint16>(), 10))
            /* possibleVelocities */,
          Region<sint16>(SphereRegionData<sint16>(Point<sint16>(), 10))
            /* possibleAngularVelocities */,
          Visibility(),
          Sensors()
        ),
        100 /* energyCost */,
        100 /* metalCost */,
        false /* fixed */,
        true /* ground */,
        false /* surface */,
        false /* air */,
        true /* seabed */,
        std::list<WeaponTypeID>(),
        NULL /* corpseUnitType */
      );
  unitTypes.push_back(commanderType);
  
  Universe universe("universe", "", unitTypes);

  /* Create the map */
  Heightfield heightfield = Heightfield();
  vector<UnitTemplate> neutralPlayersUnits;
  vector<UnitTemplate> realPlayersUnits;
  PlayerTemplate neutralPlayerTemplate =
    PlayerTemplate(true, true, realPlayersUnits);
  PlayerTemplate realPlayerTemplate =
    PlayerTemplate(false, false, neutralPlayersUnits);
  vector<PlayerTemplate> playerTemplates;
  playerTemplates.push_back(neutralPlayerTemplate);
  playerTemplates.push_back(realPlayerTemplate);
  MapPlayMode playMode = MapPlayMode(2, 2, playerTemplates);
  vector<MapPlayMode> playModes;
  playModes.push_back(playMode);
  MapTemplate t = MapTemplate(
      &universe, "Toy map", Point<uint32>(), Point<uint32>(), topology_plane,
      heightfield, 10 /* gravity */, playModes
    );

  /* For our toy example we create two players: the neutral player with no
   * client, and a "real" player with a debugging client */
  Player neutralPlayer = Player(neutralPlayerTemplate);
  Player realPlayer = Player(realPlayerTemplate);
  realPlayer.attachClient(&client);
  vector<Player> players;
  players.push_back(neutralPlayer);
  players.push_back(realPlayer);

  /* Create the world */
  World* w = new CompleteWorld(t, 0 /* mode */, players);
    
  doLoadTest(cout);
  delete w;
  w = NULL;

  /* Now another test, this time with a unit */
  cout << "Performing test with one unit each..." << endl;
  
  /* Create the map */
  heightfield = Heightfield();
  neutralPlayersUnits.push_back(
      UnitTemplate(
        &universe,
        UnitStatus(
          &universe,
          universe.getUnitTypeId(0),
          Point<sint32>(20,20,20),
          Orientation(),
          Point<sint16>()
        )
      )
    );
  realPlayersUnits.push_back(
      UnitTemplate(
        &universe,
        UnitStatus(
          &universe,
          universe.getUnitTypeId(0),
          Point<sint32>(0,0,0),
          Orientation(),
          Point<sint16>()
        )
      )
    );
  playerTemplates.clear();
  playerTemplates.push_back(PlayerTemplate(true, true, neutralPlayersUnits));
  playerTemplates.push_back(PlayerTemplate(false, false, realPlayersUnits));
  playMode = MapPlayMode(2, 2, playerTemplates);
  playModes.clear();
  playModes.push_back(playMode);
  t = MapTemplate(
      &universe, "Toy map", Point<sint32>(10000,10000,10000),
      Point<sint32>(-10000,-10000,-10000), topology_plane,
      heightfield, 10 /* gravity */, playModes
    );

  /* Create the world */
  w = new CompleteWorld(t, 0 /* mode */, players);
  
  doLoadTest(cout);
  delete w;
  w = NULL;

  /* Do a test with a unit patrolling */
  cout << "Performing test with a patrolling unit..." << endl;
  
  PatrollerClient patrollerClient(Point<sint32>(2000, 0, 0));
  players.back().attachClient(&patrollerClient);
  
  w = new CompleteWorld(t, 0 /* mode */, players);
  doLoadTest(cout);
  delete w;
  w = NULL;

  /* Done */
  cout << "All tests completed successfully." << endl;

#ifdef _MSC_VER
  cout<<"Total time taken: "<<clock()<<"ms"<<endl;
#endif

  return 0;
}

