#include "point.h"
#include "unittype.h"
#include "universe.h"
#include "heightfield.h"
#include "mapplaymode.h"
#include "planemap.h"
#include "sphereregiondata.h"
#include "libsakusen-resources-global.h"
#include "fileresourceinterface.h"
#include "resourceinterface-methods.h"

#include <iostream>

using namespace std;

using namespace sakusen;
using namespace sakusen::resources;

/** \file
 *
 * This file contains code for a test of the libsakusen-resources library.  It
 * doubles as a convinient source of toy gamedata for testing servers and
 * clients.
 *
 * The intent is to create a simple Universe and Map, save them to disk and
 * check that they can be reopened.
 */

int main(/*int argc, char** argv*/)
{
  char* homePath = getenv("HOME");
  
  cout << "Creating ResourceInterface" << endl;
  ResourceInterface* resourceInterface =
    new FileResourceInterface(String(homePath) + CONFIG_SUBDIR DATA_SUBDIR);
  
  cout << "Creating Universe" << endl;
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

  cout << "Saving Universe" << endl;
  if(resourceInterface->save(&universe))
  {
    cout << resourceInterface->getError() << endl;
    return EXIT_FAILURE;
  }

  cout << "Loading Universe" << endl;
  ResourceSearchResult result;
  Universe* reloadedUniverse =
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

  cout << "Creating map" << endl;
  Heightfield heightfield = Heightfield();
  vector<UnitTemplate> neutralPlayersUnits;
  vector<UnitTemplate> realPlayersUnits;
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
  vector<PlayerTemplate> players;
  players.push_back(PlayerTemplate(true, true, neutralPlayersUnits));
  players.push_back(PlayerTemplate(false, false, realPlayersUnits));
  MapPlayMode playMode = MapPlayMode(2, 2, players);
  vector<MapPlayMode> playModes;
  playModes.push_back(playMode);
  MapTemplate* t = new MapTemplate(
      &universe, "map", Point<sint32>(10000,10000,10000),
      Point<sint32>(-10000,-10000,-10000), topology_plane,
      heightfield, 10 /* gravity */, playModes
    );

  cout << "Saving map" << endl;
  if(resourceInterface->save(t))
  {
    cout << resourceInterface->getError() << endl;
    return EXIT_FAILURE;
  }

  cout << "Loading map" << endl;
  MapTemplate* reloadedTemplate =
    resourceInterface->search<MapTemplate>("map", reloadedUniverse, &result);
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
  
  delete reloadedTemplate;
  delete reloadedUniverse;
  delete t;
  delete resourceInterface;

  return EXIT_SUCCESS;
}

