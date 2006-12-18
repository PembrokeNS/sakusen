
#include "point.h"
#include "unittype.h"
#include "universe.h"
#include "heightfield.h"
#include "mapplaymode.h"
#include "planemap.h"
#include "sphereregiondata.h"
#include "libsakusen-resources-global.h"
#include "resourceinterface-methods.h"
#include "fileresourceinterface.h"
#include "fileutils.h"

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

/** \brief main function for test */
int main(/*int argc, char** argv*/)
{
  String homePath = fileUtils_getHome();
  String dataDir = homePath + CONFIG_SUBDIR DATA_SUBDIR;
  
  cout << "Creating ResourceInterface" << endl;
  vector<String> dataDirs;
  dataDirs.push_back(homePath + CONFIG_SUBDIR DATA_SUBDIR);
  dataDirs.push_back("data");
  dataDirs.push_back(".."FILE_SEP"data");
  dataDirs.push_back(".."FILE_SEP".."FILE_SEP"data");
  dataDirs.push_back(".."FILE_SEP".."FILE_SEP".."FILE_SEP"data");
  
  ResourceInterface::Ptr resourceInterface =
    FileResourceInterface::create(dataDirs, false);

  cout << "Cleaning out existing test files" << endl;
  list<String> universes =
    fileUtils_findMatches(dataDir + "/universe", "universe.");
  while (!universes.empty()) {
    NativeUnlink(universes.front().c_str());
    universes.pop_front();
  }
  list<String> maps =
    fileUtils_findMatches(dataDir + "/maptemplate", "map.");
  while (!maps.empty()) {
    NativeUnlink(maps.front().c_str());
    maps.pop_front();
  }
  maps = fileUtils_findMatches(dataDir + "/maptemplate", "2map.");
  while (!maps.empty()) {
    NativeUnlink(maps.front().c_str());
    maps.pop_front();
  }
  
  cout << "Creating Universe" << endl;
  vector<WeaponType> weaponTypes;
  try {
    WeaponType cannonType =
      WeaponType(
          "cannon",
          "testsrc",
          17 /* energyCost */,
          17 /* metalCost */,
          1 /* energyRate */,
          1 /* metalRate */,
          resourceInterface
        );
    weaponTypes.push_back(cannonType);
    WeaponType paralyzerType =
      WeaponType(
          "paralyzer",
          "testsrc",
          10 /* energyCost */,
          10 /* metalCost */,
          1 /* energyRate */,
          1 /* metalRate */,
          resourceInterface
        );
    weaponTypes.push_back(paralyzerType);
  } catch (DeserializationExn& e) {
    cout << "Exception: " << e.message << endl;
    return EXIT_FAILURE;
  }

  vector<UnitType> unitTypes;
  list<String> commanderWeapons;
  commanderWeapons.push_back("cannon");
  commanderWeapons.push_back("paralyzer");
  UnitType commanderType =
    UnitType(
        "commander" /* name */,
        UnitTypeData(
          100 /* maxHitPoints */,
          10 /* mass */,
          Point<uint32>(10,10,10) /* size */,
          Region<sint16>(new SphereRegionData<sint16>(Point<sint16>(), 4))
            /* possibleAccelerations */,
          Region<sint16>(new SphereRegionData<sint16>(Point<sint16>(), 10))
            /* possibleVelocities */,
          Region<sint16>(new SphereRegionData<sint16>(Point<sint16>(), 10))
            /* possibleAngularVelocities */,
          Visibility(),
          Sensors()
        ),
        100 /* energyCost */,
        100 /* metalCost */,
        false /* fixed */,
        true /* ground */,
        false /* surface */,
        true /* gravity */,
        true /* seabed */,
        commanderWeapons /* weapons */,
        "" /* corpseUnitType name */
      );
  unitTypes.push_back(commanderType);
  
  Universe universe("universe", "", weaponTypes, unitTypes);
  String name = universe.resolveNames();
  if (name != "") {
    cout << "Error resolving name '" << name << "'" << endl;
    return EXIT_FAILURE;
  }

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
#define MAP_WIDTH 1000
  Heightfield heightfield(2*MAP_WIDTH, 2, 2, 2);
  vector<UnitTemplate> neutralPlayersUnits;
  vector<UnitTemplate> realPlayersUnits;
  neutralPlayersUnits.push_back(
      UnitTemplate(
        &universe,
        UnitStatus(
          &universe,
          universe.getUnitTypeId(0),
          Point<sint32>(50,-50,0),
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
  vector<PlayerTemplate> playerTemplates;
  playerTemplates.push_back(PlayerTemplate(true, true, neutralPlayersUnits));
  playerTemplates.push_back(PlayerTemplate(false, false, realPlayersUnits));
  MapPlayMode playMode = MapPlayMode(2, 2, playerTemplates);
  vector<MapPlayMode> playModes;
  playModes.push_back(playMode);
  MapTemplate* t = new MapTemplate(
      &universe, "map", Point<sint32>(MAP_WIDTH,MAP_WIDTH,MAP_WIDTH),
      Point<sint32>(-MAP_WIDTH,-MAP_WIDTH,-MAP_WIDTH), topology_plane,
      heightfield, 2 /* gravity */, playModes
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

  cout << "Creating two-player map" << endl;
  vector<UnitTemplate> realPlayer2sUnits;
  realPlayer2sUnits.push_back(
      UnitTemplate(
        &universe,
        UnitStatus(
          &universe,
          universe.getUnitTypeId(0),
          Point<sint32>(100,-100,0),
          Orientation(),
          Point<sint16>()
        )
      )
    );
  playerTemplates.push_back(PlayerTemplate(false, false, realPlayer2sUnits));
  playMode = MapPlayMode(3, 3, playerTemplates);
  playModes.clear();
  playModes.push_back(playMode);
  delete t;
  t = new MapTemplate(
      &universe, "2map", Point<sint32>(MAP_WIDTH,MAP_WIDTH,MAP_WIDTH),
      Point<sint32>(-MAP_WIDTH,-MAP_WIDTH,-MAP_WIDTH), topology_plane,
      heightfield, 10 /* gravity */, playModes
    );

  cout << "Saving two player map" << endl;
  if (resourceInterface->save(t)) {
    cout << resourceInterface->getError() << endl;
    return EXIT_FAILURE;
  }
  
  delete reloadedTemplate;
  delete reloadedUniverse;
  delete t;

  return EXIT_SUCCESS;
}

