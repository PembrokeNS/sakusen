/* This is intended to be a quick and superficial test of libsakusen, although
 * with luck it will become less superficial over time */

#include <cerrno>
#include <iostream>

#include <time.h>

#ifdef __GNUC__
    #ifdef ENABLE_LTDL_HACKED
     #include <ltdl_hacked/ltdl_hacked.h>
    #else
     #include <ltdl.h>
    #endif
#endif

#include <sakusen/server/global.h>
#include <sakusen/point.h>
#include <sakusen/sphereregion.h>
#include <sakusen/region.h>
#include <sakusen/resourceinterface-methods.h>
#include <sakusen/fileutils.h>
#include <sakusen/resources/fileresourceinterface.h>
#include <sakusen/server/completeworld.h>
#include <sakusen/server/debuggingclient.h>
#include "patrollerclient.h"

/*This MUST be after all the other headers, due to name conflicts*/
#ifdef WIN32
  #include <windows.h>
#endif

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
    SAKUSEN_DEBUG("Clock error");
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
    SAKUSEN_DEBUG("Clock error");
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

#ifdef __GNUC__
  /* ltdl initialization */
  if (lt_dlinit()) {
    SAKUSEN_FATAL("lt_dlinit() failed");
  }
#endif

  boost::filesystem::path::default_name_check(
      boost::filesystem::portable_posix_name
    );
  boost::filesystem::path dataDir =
    fileUtils_configDirectory() / SAKUSEN_RESOURCES_SUBDIR;
  boost::filesystem::path dotDot("..");

  cout << "Creating ResourceInterface with data root " <<
    dataDir.native_directory_string() << endl;
  vector<boost::filesystem::path> dataDirs;
  dataDirs.push_back(dataDir);
  dataDirs.push_back("data");
  dataDirs.push_back(dotDot/"data");
  dataDirs.push_back(dotDot/".."/"data");
  dataDirs.push_back(dotDot/".."/".."/"data");

  ResourceInterface::Ptr resourceInterface =
    FileResourceInterface::create(dataDirs, true);

  /* Create a debugging client */
  DebuggingClient client(ClientId(), cout);
  vector<Client*> clients;
  clients.push_back(&client);

  /* Load the universe */
  cout << "Loading Universe" << endl;
  ResourceSearchResult result;
  Universe::ConstPtr universe;
  boost::tie(universe, result, boost::tuples::ignore) =
    resourceInterface->search<Universe>("test/universe");
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
  MapTemplate::Ptr mapTemplate;
  boost::tie(mapTemplate, result, boost::tuples::ignore) =
    resourceInterface->search<MapTemplate>("test/map", universe);
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
  Player neutralPlayer =
    Player(mode.getPlayer(PlayerId::fromInteger(0)), PlayerId::fromInteger(0));
  Player realPlayer =
    Player(mode.getPlayer(PlayerId::fromInteger(1)), PlayerId::fromInteger(1));
  realPlayer.attachClient(&client);
  vector<Player> players;
  players.push_back(neutralPlayer);
  players.push_back(realPlayer);

  /* Now another test, this time with a unit */
  cout << "Performing test with one unit each..." << endl;

  /* Create the world */
  World* w =
    new CompleteWorld(*mapTemplate, 0 /* mode */, players, resourceInterface);

  doLoadTest(cout);
  delete w;
  w = NULL;

  /* Do a test with a unit patrolling */
  cout << "Performing test with a patrolling unit..." << endl;

  PatrollerClient patrollerClient(
      ClientId::fromInteger(1), Point<sint32>(50000, 0, 1000)
    );
  players.back().attachClient(&patrollerClient);

  w = new CompleteWorld(*mapTemplate, 0 /* mode */, players, resourceInterface);
  doLoadTest(cout);
  delete w;
  w = NULL;

  /* Done */
  cout << "All tests completed successfully." << endl;

#ifdef _MSC_VER
  cout<<"Total time taken: "<<clock()<<"ms"<<endl;
#endif

#ifdef __GNUC__
  /* ltdl finalization */
  if (lt_dlexit()) {
    SAKUSEN_FATAL("lt_dlexit() failed");
  }
#endif

  return 0;
}

