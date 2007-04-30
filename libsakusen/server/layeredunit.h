#ifndef LIBSAKUSEN_SERVER__LAYEREDUNIT_H
#define LIBSAKUSEN_SERVER__LAYEREDUNIT_H

#include "libsakusen-global.h"
#include "libsakusen-server-global.h"

#include <list>

#include "icompleteunit.h"
#include "unitlayer.h"
#include "unittemplate.h"
#include "dynamicsensorreturns.h"
#include "unitorders.h"

namespace sakusen {
namespace server {

class UnitMask;

/** \brief A Unit together with all layers currently attached to it.
 *
 * This is a server's representation of a Unit, with all the additional data
 * that only the server gets to see, such as the layers which have been added
 * to it, SensorReturns from it to various Players, etc.
 *
 * At present the direct method of implementing layers is used - LayeredUnit
 * contains a pointer to the top layer, which has a pointer to the next one
 * down and so forth until the UnitCore is reached at the centre.
 *
 * LayeredUnit also carries a direct pointer to the Unit which is at its heart.
 * This allows direct access to certain aspects of the Unit (the physical
 * properties - position, velocity, etc. and the order queue) so that these can
 * be handled directly.  It would be bad (and slow) for layers to mess with the
 * physics too much.
 */
class LIBSAKUSEN_SERVER_API LayeredUnit :
  public ICompleteUnit, boost::noncopyable {
  public:
    typedef boost::shared_ptr<LayeredUnit> Ptr;
    typedef boost::shared_ptr<const LayeredUnit> ConstPtr;

    static Ref<LayeredUnit> spawn(
      const PlayerID owner,
      const UnitTypeID type,
      const Point<sint32>& startNear,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity,
      const HitPoints startHP = HitPoints(-1)
    ); /* factory class */
    static Ref<LayeredUnit> spawn(const PlayerID owner, const UnitTemplate& t);
      /* factory class */
  private:
    LayeredUnit(const UnitTemplate&);
      /**< Constructor used by LayeredUnit::spawn during initial construction
       * of the Map */
    LayeredUnit(
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity,
      const HitPoints startHP
    ); /* constructor intended to be used when spawning a Unit on the map -
          this is called only by LayeredUnit::spawn, and is thus private.
          owner is expected to be set after construction since it should not
          be set until *after* the Unit has been added to the World */
  public:
    ~LayeredUnit();
  private:
    PlayerID owner;
    uint32 unitId;
    UnitLayer::Ptr topLayer;
    /** \brief Shortcut pointer to the UnitStatus at the heart
     *
     * Not owned by this (except insofar as it may be equal to topLayer) */
    UnitStatus* status;

    UnitOrders orders;
    
    /** \brief Sensor returns from this unit, indexed by the player doing the
     * sensing. */
    __gnu_cxx::hash_map<PlayerID, DynamicSensorReturnsRef> sensorReturns;

    /** \brief Indicates that information must be transmitted to client
     *
     * This bool is true iff changes have occured to the unit which have yet to
     * be transmitted to the clients */
    bool dirty;

    /** Accept a new order from the queue */
    void acceptOrder(OrderCondition condition);
  public:
    /* accessors */
    inline PlayerID getOwner(void) const {return owner;}
    inline uint32 getId(void) const { return unitId; }
    inline void setId(uint32 id) { unitId = id; }
    inline const IUnitTypeData* getITypeData(void) const {
      /** \bug Using get() to save time; probably shouldn't really */
      return topLayer.get();
    }
    inline UnitStatus* getStatus(void) const { return status; }
    inline const IUnitStatus* getIStatus(void) const { return status; }
    inline const UnitOrders& getOrders(void) const { return orders; }
    inline UnitOrders& getOrders(void) { return orders; }
    inline __gnu_cxx::hash_map<PlayerID, DynamicSensorReturnsRef>&
      getSensorReturns(void) { return sensorReturns; }
    inline const __gnu_cxx::hash_map<PlayerID, DynamicSensorReturnsRef>&
      getSensorReturns(void) const { return sensorReturns; }

    void setDirty(void);
    void clearDirty(void);

    Ref<LayeredUnit> getRefToThis();
    Ref<const LayeredUnit> getRefToThis() const;
    
    void setPosition(const Point<sint32>& pos);
    bool setRadar(bool active);
    bool setSonar(bool active);
    
    /* game mechanics methods */
    void setPhysics(
        const Point<sint32>& newPosition,
        const Orientation& newOrientation,
        bool orientationIsRelative,
        bool zeroVelocity
      );
    void incrementState(const Time& timeNow);
      /* advances the state of the unit one unit of game time (changing its
			   velocity, position, applying all actions for the given time) */
    void enqueueOrder(const OrderCondition& condition, const Order& order);

    /** \brief Inserts a new layer at the top of the unit's layers */
    void insertLayer(const boost::shared_ptr<UnitMask>& layer);
    /** \brief Gets top layer of type T
     *
     * \return shared_ptr<T> to layer found, or shared_ptr<T>() if none found */
    template<typename T>
    boost::shared_ptr<T> getLayer() {
      return boost::dynamic_pointer_cast<T>(getLayer(typeid(T)));
    }

    boost::shared_ptr<UnitLayer> getLayer(const std::type_info& typeInfo) {
      if (typeInfo == typeid(*topLayer)) {
        return topLayer;
      } else {
        return topLayer->getLayer(typeInfo);
      }
    }
    /** \brief Removes a layer from amongst the unit's layers
     *
     * Fatals if that layer does not exist */
    void removeLayer(const UnitMask*);
    
    /** kills the unit unconditionally */
    inline bool kill(HitPoints excessDamage) {
      return topLayer->kill(excessDamage);
    }
    inline void damage(HitPoints amount) { topLayer->damage(amount); }
      /**< damages the unit and kills if HP <= 0 */
    inline void repair(HitPoints amount, bool superhealth) {
      topLayer->repair(amount, superhealth);
    } /**< repairs the unit up to its max HP, if superhealth is false; o/w,
       * repairs the unit up to no maximum */
    inline void changeType(
        const UnitTypeID& to,
        hitPointAlteration hpAlteration
      ) { topLayer->changeType(to, hpAlteration); }
      /**< the semantics of this are going to need some thought, but does
       * transforming of units and deployment */
    inline void changeOwner(const PlayerID to, enum changeOwnerReason why) {
      topLayer->changeOwner(to, why);
      owner = topLayer->getOwner();
    }
    inline void onDestruct(void) { topLayer->onDestruct(); }
};

}}

#endif // LIBSAKUSEN_SERVER__LAYEREDUNIT_H

