#include "ui/action.h"

#include "partialworld.h"
#include "ui/ui.h"
#include "ui/actiontarget.h"
#include "game/game.h"

#include <boost/algorithm/string/predicate.hpp>

using namespace std;
using namespace sakusen;
using namespace sakusen::client;
using namespace tedomari::game;

namespace tedomari {
namespace ui {

class MoveAction : public Action {
  public:
    MoveAction(const set<UnitID>& selection) :
      Action(actionParameterType_target),
      movers(selection),
      target()
    {
    }
  private:
    /** \brief Units to move */
    set<UnitID> movers;
    /** \brief Target to which to move them
     * (target.which()==actionTargetType_none if not yet set) */
    ActionTarget target;

    void internalSupplyArgument(const ActionArgument& arg) {
      target = boost::get<ActionTarget>(arg);
      nextParameterType = actionParameterType_none;
    }

    void internalExecute(UI* ui) {
      ui->move(movers, target);
    }
};

/* Helper class to foward attack calls */
/** \todo Replace methods with the order-queue related things */
class AttackVisitor : public boost::static_visitor<void> {
  public:
    /** \warning Stores a reference to its second argument */
    AttackVisitor(Game* g, const set<UnitID>& un) :
      game(g),
      units(un)
    {}
  private:
    Game* game;
    const set<UnitID>& units;
  public:

  void operator()(const Point<sint32>& target);
  void operator()(boost::blank) {
    Fatal("invalid target");
  }
};

void AttackVisitor::operator()(const Point<sint32>& target)
{
  PartialWorld::UnitIDIndex::ConstPtr unitIndex =
    client::world->getUnitsById();
  for (set<UnitID>::const_iterator unitID = units.begin();
      unitID != units.end(); ++unitID) {
    Ref<UpdatedUnit> unit = unitIndex->find(*unitID);
    const UnitType* type =
      client::world->getUniverse()->getUnitTypePtr(unit->getStatus().getType());
    /* For each weapon, if it's a violent weapon then aim it at the target */
    const vector<WeaponTypeID>& weapons = type->getWeapons();
    for (vector<WeaponTypeID>::const_iterator weaponTypeID = weapons.begin();
        weaponTypeID != weapons.end(); ++weaponTypeID) {
      const WeaponType* weaponType =
        client::world->getUniverse()->getWeaponTypePtr(*weaponTypeID);
      if (weaponType->getClientHint() == "o") {
        Order order(
            new TargetPositionOrderData(weaponTypeID-weapons.begin(), target)
          );
        this->game->order(OrderMessage(*unitID, order));
      }
    }
  }
}

class AttackAction : public Action {
  public:
    AttackAction(const set<UnitID>& selection) :
      Action(actionParameterType_target),
      attackers(selection),
      target()
    {
    }
  private:
    /** \brief Units to attack with */
    set<UnitID> attackers;
    /** \brief Target at which to attack them
     * (target.which()==actionTargetType_none if not yet set) */
    ActionTarget target;

    void internalSupplyArgument(const ActionArgument& arg) {
      target = boost::get<ActionTarget>(arg);
      nextParameterType = actionParameterType_none;
    }

    void internalExecute(UI* ui) {
      AttackVisitor attackVisitor(ui->getGame(), attackers);
      target.apply_visitor(attackVisitor);
    }
};

class CreateAction : public Action {
  public:
    CreateAction(const set<UnitID>& selection) :
      Action(actionParameterType_stringFromSet),
      creaters(selection)
    {
      /* Extract the set of all unit types represented in the selection */
      set<UnitTypeID> unitTypes;
      for (set<UnitID>::iterator createrId = creaters.begin();
          createrId != creaters.end(); ++createrId) {
        Ref<UpdatedUnit> creater =
          client::world->getUnitsById()->find(*createrId);
        unitTypes.insert(creater->getStatus().getType());
      }
      /* Extract all the types of things any of these unit types can build */
      for (set<UnitTypeID>::iterator typeId = unitTypes.begin();
          typeId != unitTypes.end(); ++typeId) {
        const UnitType* unitType =
          client::world->getUniverse()->getUnitTypePtr(*typeId);
        const vector<WeaponTypeID>& weapons = unitType->getWeapons();
        for (vector<WeaponTypeID>::const_iterator weaponTypeID =
            weapons.begin(); weaponTypeID != weapons.end(); ++weaponTypeID) {
          const WeaponType* weaponType =
            client::world->getUniverse()->getWeaponTypePtr(*weaponTypeID);
          if (boost::starts_with(weaponType->getClientHint(), "c:")) {
            possibleCreations.insert(weaponType->getClientHint().substr(2));
          }
        }
      }
    }
  private:
    /** \brief Units to create with */
    set<UnitID> creaters;
    set<String> possibleCreations;
    String creation;
    pair<Position,Orientation> target;

    void internalSupplyArgument(const ActionArgument& arg) {
      switch (nextParameterType) {
        case actionParameterType_stringFromSet:
          {
            const String& c = boost::get<String>(arg);
            set<String>::iterator i = possibleCreations.find(c);
            if (i == possibleCreations.end()) {
              throw ActionArgumentExn(
                  "string '"+c+"' not one of the valid values"
                );
            } else {
              creation = c;
              nextParameterType = actionParameterType_positionOrientation;
            }
          }
          break;
        case actionParameterType_positionOrientation:
          {
            target = boost::get<pair<Position, Orientation> >(arg);
            nextParameterType = actionParameterType_none;
          }
          break;
        default:
          Fatal("unexpected parameter type");
      }
    }

    void internalExecute(UI*);

    const set<String>& internalGetStringSet() const {
      return possibleCreations;
    }
};

void CreateAction::internalExecute(UI* ui) {
  for (set<UnitID>::iterator createrId = creaters.begin();
      createrId != creaters.end(); ++createrId) {
    Ref<UpdatedUnit> creater =
      client::world->getUnitsById()->find(*createrId);
    if (!creater.isValid()) {
      continue;
    }
    const UnitType* unitType =
      client::world->getUniverse()->
      getUnitTypePtr(creater->getStatus().getType());
    const vector<WeaponTypeID>& weapons = unitType->getWeapons();
    for (vector<WeaponTypeID>::const_iterator weaponTypeID =
        weapons.begin(); weaponTypeID != weapons.end(); ++weaponTypeID) {
      const WeaponType* weaponType =
        client::world->getUniverse()->getWeaponTypePtr(*weaponTypeID);
      if (weaponType->getClientHint() == "c:"+creation) {
        Order order(new TargetPositionOrientationOrderData(
              weaponTypeID-weapons.begin(), target
            ));
        ui->getGame()->order(OrderMessage(*createrId, order));
        return;
      }
    }
  }
  /* If we've got this far, then we can't find a unit capable of building the
   * desired building (presumably because all those that were capable have dies
   * recently), so we alert the user */
  /** \bug Shouldn't use creation - that's the internal name, not the
   * user-friendly name */
  ui->alert(Alert("Construction of '"+creation+"' not possible"));
}

class BuildAction : public Action {
  public:
    BuildAction(const set<UnitID>& selection) :
      Action(actionParameterType_unit),
      builders(selection),
      target()
    {
    }
  private:
    /** \brief Units to build with */
    set<UnitID> builders;
    /** \brief Target to build
     * (!target.isValid() if not yet set) */
    Ref<UpdatedUnit> target;

    void internalSupplyArgument(const ActionArgument& arg) {
      target = boost::get<Ref<UpdatedUnit> >(arg);
      nextParameterType = actionParameterType_none;
    }

    void internalExecute(UI* ui) {
      PartialWorld::UnitIDIndex::ConstPtr unitIndex =
        client::world->getUnitsById();
      for (set<UnitID>::const_iterator unitID = builders.begin();
          unitID != builders.end(); ++unitID) {
        Ref<UpdatedUnit> unit = unitIndex->find(*unitID);
        const UnitType* type =
          client::world->getUniverse()->
          getUnitTypePtr(unit->getStatus().getType());
        /* For each weapon, if it's a builder then aim it at the target */
        const vector<WeaponTypeID>& weapons = type->getWeapons();
        for (vector<WeaponTypeID>::const_iterator weaponTypeID =
            weapons.begin(); weaponTypeID != weapons.end(); ++weaponTypeID) {
          const WeaponType* weaponType =
            client::world->getUniverse()->getWeaponTypePtr(*weaponTypeID);
          if (weaponType->getClientHint() == "b") {
            Order order(
                new TargetUnitOrderData(weaponTypeID-weapons.begin(), target)
              );
            ui->getGame()->order(
                OrderMessage(*unitID, order)
              );
          }
        }
      }
    }
};

/** \brief Creates a new Action based on its name and the present selection
 *
 * \param actionName Name of Action to produce
 * \param selection Selection to use as a basis for action (might be actors,
 *    target or something else)
 * \return Action::Ptr pointing to Action created, or Action::Ptr() if no
 *    action of that name exists
 */
Action::Ptr initializeAction(
    const String& actionName,
    const set<UnitID>& selection
  )
{
  /** \bug Possibly slow use of if statements; could use a hashtable. */
  if (actionName == "move") {
    return Action::Ptr(new MoveAction(selection));
  } else if (actionName == "attack") {
    return Action::Ptr(new AttackAction(selection));
  } else if (actionName == "create") {
    return Action::Ptr(new CreateAction(selection));
  } else if (actionName == "build") {
    return Action::Ptr(new BuildAction(selection));
  } else {
    return Action::Ptr();
  }
}

}}

