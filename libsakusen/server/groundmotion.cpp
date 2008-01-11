#include "groundmotion.h"

#include "unitstatus-methods.h"
#include "completeworld.h"

namespace sakusen {
namespace server {

void GroundMotion::incrementState(LayeredUnit& unit)
{
  UnitStatus& status = unit.getStatus();
  const UnitOrders& orders = unit.getOrders();
  const IUnitTypeData& typeData = unit.getITypeData();

  Point<sint16> expectedVelocity(status.velocity);
  
  /* compute the expected velocity based on the unit's orders */
  switch (orders.getLinearTarget()) {
    case linearTargetType_none:
      break;
    case linearTargetType_velocity:
      if (typeData.getPossibleVelocities()->contains(
            orders.getTargetVelocity()
          )) {
        Point<sint16> desiredVelocity = orders.getTargetVelocity();
        Point<sint16> acceleration = desiredVelocity - status.velocity;
        acceleration =
          typeData.getPossibleAccelerations()->truncateToFit(acceleration);
        expectedVelocity += acceleration;
      }
      break;
    case linearTargetType_position:
      {
        Point<sint32> desiredDirection =
          world->getMap()->getShortestDifference(
              orders.getTargetPosition(), status.position
            );
        Point<sint16> desiredVelocity(
            typeData.getPossibleVelocities()->truncateToFit(desiredDirection)
          );
        Point<sint16> acceleration = desiredVelocity - status.velocity;
        acceleration =
          typeData.getPossibleAccelerations()->truncateToFit(acceleration);
        
        /*if (owner == 1 && unitId == 0) {
          Debug("[1] desiredVel=" << desiredVelocity <<
              ", acc=" << acceleration);
        }*/
        
        expectedVelocity += acceleration;
      }
      break;
    default:
      Fatal("Unknown linearTargetType '" << orders.getLinearTarget() << "'");
      break;
  }

  /* Update the expected velocity to take account of gravity and the ground. */
  /** \todo Replace this extremely crude collision detection with the ground
   * with something more sane */
  Box<sint32> boundingBox(unit.getBoundingBox());
  sint32 groundHeight = world->getCompleteMap()->getHeightfield().
    getMaxHeightIn(boundingBox.rectangle());
  sint32 heightAboveGround = boundingBox.getMin().z - groundHeight;
  if (heightAboveGround + expectedVelocity.z < 0) {
    /* The unit will end up below the ground, so we need to raise it. */
    /** \bug The
     * way this is done at pesent is quite silly and could easily result in
     * exceptionally high velocities. (This is the same bug that lots of
     * speedruns exploit in other games to achieve high speeds) */
    expectedVelocity.z = -heightAboveGround;
  } else if (heightAboveGround > 0) {
    /* The unit is in the air above the ground, so we need to apply gravity
     * */
    expectedVelocity.z -= world->getMap()->getGravity();
    /* But at the same time ensure that we don't push the unit underground */
    if (expectedVelocity.z < -heightAboveGround) {
      expectedVelocity.z = -heightAboveGround;
    }
  }

  /*if (owner == 1 && unitId == 0) {
    Debug("[2] heightAboveGround=" << heightAboveGround <<
        ", expectedVelocity=" << expectedVelocity);
  }*/

  /* Now set the velocity to this newly computed value */
  if (expectedVelocity != status.velocity) {
    status.velocity = expectedVelocity;
    unit.setDirty();
  }
  
  /** \todo Do collision detection. */
  Orientation mapOrientationChange;
  unit.setPosition(world->getMap()->addToPosition(
        status.position, status.velocity, &mapOrientationChange
      ));
  /* If the movement caused us to rotate/reflect (due to moving over a map
   * edge) then update orientation and velocity appropriately */
  if (mapOrientationChange != Orientation()) {
    status.velocity = mapOrientationChange * status.velocity;
    status.orientation = mapOrientationChange * status.orientation;
  }
}

}}

