#include "point.h"
#include "unittype.h"
#include "universe.h"
#include "heightfield.h"
#include "unit.h"
#include "mapplaymode.h"
#include "planemap.h"
#include "libsakusen-resources-global.h"
#include "fileresourceinterface.h"
#include "resourceinterface-methods.h"

#include <iostream>

using namespace std;

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
  vector<Unit> neutralPlayersUnits;
  vector<Unit> realPlayersUnits;
  realPlayersUnits.push_back(
      Unit(
        universe.getUnitTypeId(0),
        Point<sint32>(1,1,1),
        Orientation(),
        Point<sint16>(),
        &universe
      )
    );
  vector< vector<Unit> > playersUnits;
  playersUnits.push_back(neutralPlayersUnits);
  playersUnits.push_back(realPlayersUnits);
  MapPlayMode playMode = MapPlayMode(2, 2, playersUnits);
  vector<MapPlayMode> playModes;
  playModes.push_back(playMode);
  Map* map = new PlaneMap(
      &universe, "map", Point<uint32>(10000,10000,10000),
      heightfield, 10 /* gravity */, playModes
    );

  cout << "Saving map" << endl;
  if(resourceInterface->save(map))
  {
    cout << resourceInterface->getError() << endl;
    return EXIT_FAILURE;
  }

  cout << "Loading map" << endl;
  Map* reloadedMap =
    resourceInterface->search<Map>("map", reloadedUniverse, &result);
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
  
  delete reloadedMap;
  delete reloadedUniverse;
  delete map;
  delete resourceInterface;

  return EXIT_SUCCESS;
}

