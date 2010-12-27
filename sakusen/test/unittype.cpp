#include <sakusen/unittype.h>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <sakusen/sphereregion.h>

namespace sakusen { namespace test {

BOOST_AUTO_TEST_CASE(constructs)
{
  Sensors opticalSensors;
  opticalSensors.optical.capable = true;
  opticalSensors.optical.range = 30000;
  std::list<String> commanderWeapons;
  Region<sint16>::Ptr possibleAccelerations(
    new SphereRegion<sint16>(Point<sint16>(), 400)
  );
  Region<sint16>::Ptr possibleVelocities(
    new SphereRegion<sint16>(Point<sint16>(), 1000)
  );
  Region<sint32>::Ptr possibleAngularVelocities(
    new SphereRegion<sint32>(AngularVelocity(), 100000)
  );
  UnitTypeData typeData(
    HitPoints(100) /* maxHitPoints */,
    10 /* mass */,
    Point<uint32>(1000,1000,1000) /* size */,
    possibleAccelerations,
    possibleVelocities,
    possibleAngularVelocities,
    Visibility(),
    opticalSensors
  );

  /* This constructor call segfaults on some versions of clang */
  UnitType type =
    UnitType(
        "commander" /* name */,
        typeData,
        100 /* energyCost */,
        100 /* metalCost */,
        "ground" /* motionType */,
        commanderWeapons /* weapons */,
        "" /* corpseUnitType name */
      );
}

}}

