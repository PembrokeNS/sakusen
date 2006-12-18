/* This is intended to be a quick and superficial test of libsakusen, although
 * with luck it will become less superficial over time */

#include "libsakusen-global.h"
#include "point.h"
#include "sphereregiondata.h"
#include "region.h"
#include "resourceinterface-methods.h"
#include "planemap.h"
#include "fileutils.h"
#include "fileresourceinterface.h"
#include "completeworld.h"
#include "debuggingclient.h"
#include "patrollerclient.h"

#include <time.h>
#include <ltdl_hacked.h>

#include <iostream>
#include <cerrno>

#define NANO 1000000000 /* Nanoseconds in a second */

using namespace std;

using namespace sakusen;
using namespace sakusen::resources;
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
    cout << "Tickrate: "<<1000 * numTicks / timeTaken<<" tps "<<endl;
  } 
  else {
    cout<<"Test too fast to time."<<endl;
  }
#endif
}

int main(/* int argc, char** argv */)
{
  /* ltdl initialization */
  if (lt_dlinit()) {
    Fatal("lt_dlinit() failed");
  }
  
  String homePath = fileUtils_getHome();
  String dataDir = homePath + CONFIG_SUBDIR DATA_SUBDIR;
  
  cout << "Creating ResourceInterface with data root " << dataDir << endl;
  vector<String> dataDirs;
  dataDirs.push_back(homePath + CONFIG_SUBDIR DATA_SUBDIR);
  dataDirs.push_back("data");
  dataDirs.push_back(".."FILE_SEP"data");
  dataDirs.push_back(".."FILE_SEP".."FILE_SEP"data");
  dataDirs.push_back(".."FILE_SEP".."FILE_SEP".."FILE_SEP"data");
  
  ResourceInterface::Ptr resourceInterface =
    FileResourceInterface::create(dataDirs, true);
  
  /* Create a debugging client */
  DebuggingClient client(cout);
  vector<Client*> clients;
  clients.push_back(&client);

  /* Load the universe */
  cout << "Loading Universe" << endl;
  ResourceSearchResult result;
  Universe* universe =
    resourceInterface->search<Universe>("universe", NULL, &result);
  cout << "Result of reload was " << result << endl;
  
  switch(result) {
    case resourceSearchResult_error:
      cout << "Error was: " << resourceInterface->getError() << endl;
    case resourceSearchResult_notFound:
    case resourceSearchResult_ambiguous:
      return EXIT_FAILURE;
    default:
      break;
  }

  /* Load the map */
  cout << "Loading Map" << endl;
  MapTemplate* mapTemplate =
    resourceInterface->search<MapTemplate>("map", universe, &result);
  cout << "Result of reload was " << result << endl;
  
  switch(result) {
    case resourceSearchResult_error:
      cout << "Error was: " << resourceInterface->getError() << endl;
    case resourceSearchResult_notFound:
    case resourceSearchResult_ambiguous:
      return EXIT_FAILURE;
    default:
      break;
  }

  /* For our toy example we create two players: the neutral player with no
   * client, and a "real" player with a debugging client */
  const MapPlayMode& mode = mapTemplate->getPlayModes()[0];
  Player neutralPlayer = Player(mode.getPlayer(0));
  Player realPlayer = Player(mode.getPlayer(1));
  realPlayer.attachClient(&client);
  vector<Player> players;
  players.push_back(neutralPlayer);
  players.push_back(realPlayer);

  /* Now another test, this time with a unit */
  cout << "Performing test with one unit each..." << endl;
  
  /* Create the world */
  World* w = new CompleteWorld(*mapTemplate, 0 /* mode */, players);
  
  doLoadTest(cout);
  delete w;
  w = NULL;

  /* Do a test with a unit patrolling */
  cout << "Performing test with a patrolling unit..." << endl;
  
  PatrollerClient patrollerClient(Point<sint32>(500, 0, 10));
  players.back().attachClient(&patrollerClient);
  
  w = new CompleteWorld(*mapTemplate, 0 /* mode */, players);
  doLoadTest(cout);
  delete w;
  w = NULL;

  /* Done */
  cout << "All tests completed successfully." << endl;

#ifdef _MSC_VER
  cout<<"Total time taken: "<<clock()<<"ms"<<endl;
#endif
  
  delete mapTemplate;
  delete universe;
  
  /* ltdl finalization */
  if (lt_dlexit()) {
    Fatal("lt_dlexit() failed");
  }

  return 0;
}

