
#include "point.h"
#include "unittype.h"
#include "universe.h"
#include "heightfield.h"
#include "mapplaymode.h"
#include "planemap.h"
#include "sphereregion.h"
#include "libsakusen-resources-global.h"
#include "resourceinterface-methods.h"
#include "fileresourceinterface.h"
#include "fileutils.h"
#include "pngimage.h"
#include "fileioexn.h"

#include <iostream>
#include <boost/filesystem/operations.hpp>

using namespace std;

using namespace sakusen;
using namespace sakusen::resources;

/** \file
 *
 * This file contains code for a test of the libsakusen-resources library.  It
 * doubles as a convenient source of toy gamedata for testing servers and
 * clients.
 *
 * The intent is to create a simple Universe and Map, save them to disk and
 * check that they can be reopened.
 */

/** \brief main function for test */
int main(/*int argc, char** argv*/)
{
  boost::filesystem::path::default_name_check(
      boost::filesystem::portable_posix_name
    );
  boost::filesystem::path homePath = fileUtils_getHome();
  boost::filesystem::path dataDir = homePath / CONFIG_SUBDIR / DATA_SUBDIR;
  boost::filesystem::path testDir = dataDir / "test";
  fileUtils_mkdirRecursive(dataDir);
  boost::filesystem::path dotDot("..");
  
  cout << "Creating ResourceInterface" << endl;
  vector<boost::filesystem::path> dataDirs;
  dataDirs.push_back(dataDir);
  dataDirs.push_back("data");
  dataDirs.push_back(dotDot/"data");
  dataDirs.push_back(dotDot/".."/"data");
  dataDirs.push_back(dotDot/".."/".."/"data");
  dataDirs.push_back(dotDot/".."/".."/".."/"data");
  
  ResourceInterface::Ptr resourceInterface =
    FileResourceInterface::create(dataDirs, false);

  if (boost::filesystem::exists(testDir)) {
    cout << "Cleaning out existing test files" << endl;
    //Be a bit less zealous in deletion, and notably leave heightfields around.
    list<boost::filesystem::path> files =
      fileUtils_findMatches(testDir, ".sakusenmaptemplate");
    while (!files.empty()) {
      boost::filesystem::remove(files.front());
      files.pop_front();
    }
    //Why am I doing it like this? Because the + operator is not defined for list<path>
    files =
      fileUtils_findMatches(testDir, ".sakusenuniverse");
    while (!files.empty()) {
      boost::filesystem::remove(files.front());
      files.pop_front();
    }
  }
  
  cout << "Creating Universe" << endl;
  vector<WeaponType> weaponTypes;
  try {
    WeaponType cannonType =
      WeaponType(
          "cannon",
          "test/testsrc",
          "o" /* clientHint */,
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
          "test/testsrc",
          "o" /* clientHint */,
          10 /* energyCost */,
          10 /* metalCost */,
          1 /* energyRate */,
          1 /* metalRate */,
          resourceInterface
        );
    weaponTypes.push_back(paralyzerType);
    WeaponType factorycreaterType =
      WeaponType(
          "factorycreater",
          "test/testsrc",
          "c:factory" /* clientHint */,
          10 /* energyCost */,
          10 /* metalCost */,
          1 /* energyRate */,
          1 /* metalRate */,
          resourceInterface
        );
    weaponTypes.push_back(factorycreaterType);
    WeaponType gruntcreaterType =
      WeaponType(
          "gruntcreater",
          "test/testsrc",
          "c:grunt" /* clientHint */,
          10 /* energyCost */,
          10 /* metalCost */,
          1 /* energyRate */,
          1 /* metalRate */,
          resourceInterface
        );
    weaponTypes.push_back(gruntcreaterType);
    WeaponType spidercreaterType =
      WeaponType(
          "spidercreater",
          "test/testsrc",
          "c:spider" /* clientHint */,
          10 /* energyCost */,
          10 /* metalCost */,
          1 /* energyRate */,
          1 /* metalRate */,
          resourceInterface
        );
    weaponTypes.push_back(spidercreaterType);
    WeaponType builderType =
      WeaponType(
          "builder",
          "test/testsrc",
          "b" /* clientHint */,
          10 /* energyCost */,
          10 /* metalCost */,
          1 /* energyRate */,
          1 /* metalRate */,
          resourceInterface
        );
    weaponTypes.push_back(builderType);
  } catch (DeserializationExn& e) {
    cout << "Exception: " << e.message << endl;
    return EXIT_FAILURE;
  }

  vector<UnitType> unitTypes;
  list<String> commanderWeapons;
  commanderWeapons.push_back("cannon");
  commanderWeapons.push_back("paralyzer");
  commanderWeapons.push_back("factorycreater");
  commanderWeapons.push_back("builder");
  /* The following use of shared_ptr temporaries could lead to a memory leak in
   * extreme circumstances, but since this is only a test I don't really care
   * */
  UnitType commanderType =
    UnitType(
        "commander" /* name */,
        UnitTypeData(
          HitPoints(100) /* maxHitPoints */,
          10 /* mass */,
          Point<uint32>(1000,1000,1000) /* size */,
          Region<sint16>::Ptr(new SphereRegion<sint16>(Point<sint16>(), 400))
            /* possibleAccelerations */,
          Region<sint16>::Ptr(new SphereRegion<sint16>(Point<sint16>(), 1000))
            /* possibleVelocities */,
          Region<sint16>::Ptr(new SphereRegion<sint16>(Point<sint16>(), 1000))
            /* possibleAngularVelocities */,
          Visibility(),
          Sensors()
        ),
        100 /* energyCost */,
        100 /* metalCost */,
        "ground" /* motionType */,
        commanderWeapons /* weapons */,
        "" /* corpseUnitType name */
      );
  unitTypes.push_back(commanderType);

  list<String> factoryWeapons;
  factoryWeapons.push_back("gruntcreater");
  factoryWeapons.push_back("spidercreater");
  factoryWeapons.push_back("builder");
  UnitType factoryType =
    UnitType(
        "factory" /* name */,
        UnitTypeData(
          HitPoints(100) /* maxHitPoints */,
          10 /* mass */,
          Point<uint32>(5000,5000,5000) /* size */,
          Region<sint16>::Ptr(new SphereRegion<sint16>(Point<sint16>(), 0))
            /* possibleAccelerations */,
          Region<sint16>::Ptr(new SphereRegion<sint16>(Point<sint16>(), 0))
            /* possibleVelocities */,
          Region<sint16>::Ptr(new SphereRegion<sint16>(Point<sint16>(), 0))
            /* possibleAngularVelocities */,
          Visibility(),
          Sensors()
        ),
        100 /* energyCost */,
        100 /* metalCost */,
        "fixed" /* motionType */,
        factoryWeapons /* weapons */,
        "" /* corpseUnitType name */
      );
  unitTypes.push_back(factoryType);

  list<String> gruntWeapons;
  gruntWeapons.push_back("cannon");
  UnitType gruntType =
    UnitType(
        "grunt" /* name */,
        UnitTypeData(
          HitPoints(10) /* maxHitPoints */,
          10 /* mass */,
          Point<uint32>(500,500,500) /* size */,
          Region<sint16>::Ptr(new SphereRegion<sint16>(Point<sint16>(), 500))
            /* possibleAccelerations */,
          Region<sint16>::Ptr(new SphereRegion<sint16>(Point<sint16>(), 500))
            /* possibleVelocities */,
          Region<sint16>::Ptr(new SphereRegion<sint16>(Point<sint16>(), 500))
            /* possibleAngularVelocities */,
          Visibility(),
          Sensors()
        ),
        100 /* energyCost */,
        100 /* metalCost */,
        "ground" /* motionType */,
        gruntWeapons /* weapons */,
        "" /* corpseUnitType name */
      );
  unitTypes.push_back(gruntType);

  list<String> spiderWeapons;
  spiderWeapons.push_back("paralyzer");
  UnitType spiderType =
    UnitType(
        "spider" /* name */,
        UnitTypeData(
          HitPoints(10) /* maxHitPoints */,
          10 /* mass */,
          Point<uint32>(2000,2000,250) /* size */,
          Region<sint16>::Ptr(new SphereRegion<sint16>(Point<sint16>(), 500))
            /* possibleAccelerations */,
          Region<sint16>::Ptr(new SphereRegion<sint16>(Point<sint16>(), 500))
            /* possibleVelocities */,
          Region<sint16>::Ptr(new SphereRegion<sint16>(Point<sint16>(), 500))
            /* possibleAngularVelocities */,
          Visibility(),
          Sensors()
        ),
        100 /* energyCost */,
        100 /* metalCost */,
        "ground" /* motionType */,
        spiderWeapons /* weapons */,
        "" /* corpseUnitType name */
      );
  unitTypes.push_back(spiderType);

  Universe::Ptr universe(new Universe("universe", "", weaponTypes, unitTypes));
  String name = universe->resolveNames();
  if (name != "") {
    cout << "Error resolving name '" << name << "'" << endl;
    return EXIT_FAILURE;
  }

  cout << "Saving Universe" << endl;
  if(resourceInterface->save(Universe::ConstPtr(universe), "test"))
  {
    cout << resourceInterface->getError() << endl;
    return EXIT_FAILURE;
  }

  cout << "Loading Universe" << endl;
  ResourceSearchResult result;
  Universe::ConstPtr reloadedUniverse;
  boost::tie(reloadedUniverse, result, boost::tuples::ignore) =
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

  cout << "Creating map" << endl;
#define MAP_WIDTH 100000
  Heightfield heightfield(2*MAP_WIDTH, 2, 2, 2);
  vector<UnitTemplate> neutralPlayersUnits;
  vector<UnitTemplate> realPlayersUnits;
  neutralPlayersUnits.push_back(
      UnitTemplate(
        universe,
        UnitStatus(
          universe,
          universe->getUnitTypeId(0),
          Point<sint32>(5000,-5000,0),
          Orientation(),
          Point<sint16>()
        )
      )
    );
  realPlayersUnits.push_back(
      UnitTemplate(
        universe,
        UnitStatus(
          universe,
          universe->getUnitTypeId(0),
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
  MapTemplate::ConstPtr t(new MapTemplate(
        universe, "map", Point<sint32>(MAP_WIDTH,MAP_WIDTH,MAP_WIDTH),
        Point<sint32>(-MAP_WIDTH,-MAP_WIDTH,-MAP_WIDTH), topology_plane,
        heightfield, 200 /* gravity */, playModes
      ));

  cout << "Saving map" << endl;
  if(resourceInterface->save(t, "test"))
  {
    cout << resourceInterface->getError() << endl;
    return EXIT_FAILURE;
  }

  cout << "Loading map" << endl;
  MapTemplate::Ptr reloadedTemplate;
  boost::tie(reloadedTemplate, result, boost::tuples::ignore) =
    resourceInterface->search<MapTemplate>("test/map", reloadedUniverse);
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
        universe,
        UnitStatus(
          universe,
          universe->getUnitTypeId(0),
          Point<sint32>(10000,-10000,0),
          Orientation(),
          Point<sint16>()
        )
      )
    );
  playerTemplates.push_back(PlayerTemplate(false, false, realPlayer2sUnits));
  playMode = MapPlayMode(3, 3, playerTemplates);
  playModes.clear();
  playModes.push_back(playMode);
  t.reset(new MapTemplate(
        universe, "2map", Point<sint32>(MAP_WIDTH,MAP_WIDTH,MAP_WIDTH),
        Point<sint32>(-MAP_WIDTH,-MAP_WIDTH,-MAP_WIDTH), topology_plane,
        heightfield, 1000 /* gravity */, playModes
      ));

  cout << "Saving two player map" << endl;
  if (resourceInterface->save(t, "test")) {
    cout << resourceInterface->getError() << endl;
    return EXIT_FAILURE;
  }

  /* Now another one-or-two player map, but this time using a heightfield
   * built from an image */
  playerTemplates.pop_back();
  playMode = MapPlayMode(2, 2, playerTemplates);
  playModes.clear();
  playModes.push_back(playMode);

  cout<< "Loading Image"<<endl;
  Image::Ptr image;
  String imagePath;
  boost::tie(image, imagePath) =
    resourceInterface->imageSearch("test/heightfield1");
  if (!image) {
    cout << "Failed to load image" << endl;
    return EXIT_FAILURE;
  }
  heightfield = Heightfield(2*MAP_WIDTH/100, 256, imagePath, image);
  t.reset(new MapTemplate(
        universe, "immap", Point<sint32>(MAP_WIDTH,MAP_WIDTH,MAP_WIDTH),
        Point<sint32>(-MAP_WIDTH,-MAP_WIDTH,-MAP_WIDTH), topology_plane,
        heightfield, 1000 /* gravity */, playModes
      ));
  
  cout << "Saving image-based map" << endl;
  if (resourceInterface->save(t, "test")) {
    cout << resourceInterface->getError() << endl;
    return EXIT_FAILURE;
  }

  cout << "Reloading image-based map" << endl;
  boost::tie(reloadedTemplate, result, boost::tuples::ignore) =
    resourceInterface->search<MapTemplate>("test/immap", reloadedUniverse);
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
  return EXIT_SUCCESS;
}

