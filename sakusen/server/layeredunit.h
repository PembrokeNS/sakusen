#ifndef LIBSAKUSEN_SERVER__LAYEREDUNIT_H
#define LIBSAKUSEN_SERVER__LAYEREDUNIT_H

#include <sakusen/server/global.h>

#include <list>
#include <boost/scoped_ptr.hpp>

#include <sakusen/icompleteunit.h>
#include <sakusen/server/unitlayer.h>
#include <sakusen/unittemplate.h>
#include <sakusen/server/dynamicsensorreturns.h>
#include <sakusen/unitorders.h>
#include <sakusen/server/unitmotion.h>

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
  public ICompleteUnit, private boost::noncopyable {
  public:
    typedef boost::shared_ptr<LayeredUnit> Ptr;
    typedef boost::shared_ptr<const LayeredUnit> ConstPtr;

    static Ref<LayeredUnit> spawn(
      const PlayerId owner,
      const UnitTypeId type,
      const Frame& startNear,
      const Point<sint16>& startVelocity,
      const HitPoints startHP = HitPoints(0)
    );
    static Ref<LayeredUnit> spawn(const PlayerId owner, const UnitTemplate& t);
  private:
    /** Constructor used by LayeredUnit::spawn during initial construction
     * of the Map */
    LayeredUnit(const UnitTemplate&);
    /** Constructor intended to be used when spawning a Unit on the map -
     * this is called only by LayeredUnit::spawn, and is thus private.
     * \p owner is expected to be set after construction since it should not
     * be set until *after* the Unit has been added to the World. */
    LayeredUnit(
      const UnitTypeId& startType,
      const Frame& startFrame,
      const Point<sint16>& startVelocity,
      const HitPoints startHP
    );
    /** Another version of the constructor for the other
     * LayeredUnit::spawn. */
    LayeredUnit(
      const UnitTypeId& startType,
      const Frame& startFrame,
      const Point<sint16>& startVelocity
    );
  public:
    ~LayeredUnit();
  private:
    PlayerId owner;
    UnitId unitId;
    UnitLayer::Ptr topLayer;
    /** \brief Shortcut pointer to the UnitStatus at the heart
     *
     * Not owned by this (except insofar as it may be equal to topLayer) */
    UnitStatus& status;

    /** \brief Object that controlls the way this unit moves around.
     *
     * This includes the  import of gravity, water, etc. */
    boost::scoped_ptr<UnitMotion> motion;

    UnitOrders orders;
    
    /** \brief Sensor returns from this unit, indexed by the player doing the
     * sensing. */
    u_map<PlayerId, DynamicSensorReturnsRef>::type sensorReturns;

    /** \brief Indicates that information must be transmitted clients.
     *
     * This bool is true iff changes have occured to the unit which have yet to
     * be transmitted to the clients. */
    bool dirty;
  public:
    /** \name Accessors. */
    //@{
    inline PlayerId getOwner(void) const {return owner;}
    inline UnitId getId(void) const { return unitId; }
    inline void setId(UnitId id) { unitId = id; }
    inline const IUnitTypeData& getITypeData(void) const { return *topLayer; }
    inline UnitStatus& getStatus(void) const { return status; }
    inline const IUnitStatus& getIStatus(void) const { return status; }
    inline const UnitOrders& getOrders(void) const { return orders; }
    inline UnitOrders& getOrders(void) { return orders; }
    inline u_map<PlayerId, DynamicSensorReturnsRef>::type&
      getSensorReturns(void) { return sensorReturns; }
    inline const u_map<PlayerId, DynamicSensorReturnsRef>::type&
      getSensorReturns(void) const { return sensorReturns; }
    //@}

    void setDirty(void);
    void clearDirty(void);

    Ref<LayeredUnit> getRefToThis();
    Ref<const LayeredUnit> getRefToThis() const;
    
    void setPosition(const Position&);
    bool setRadar(bool active);
    bool setSonar(bool active);

    static void incrementState(Ref<LayeredUnit> refToThis, const Time& timeNow);

    /** Accept a new order from the queue */
    void acceptOrder(const Order&);

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
    void removeLayer(const UnitMask*);
    
    /** \brief Kills the unit unconditionally.
     *
     * \return true iff the unit is gone (as
     * opposed to, e.g., surviving because of
     * some special property or being turned into a corpse). */
    inline bool kill(HitPoints excessDamage = HitPoints(0)) {
      return topLayer->kill(excessDamage);
    }
    /** \brief Damages the unit and kills if HP <= 0 */
    inline void damage(HitPoints amount) { topLayer->damage(amount); }
    
    /** \brief Repairs the unit.
     *
     * If superhealth is false, then repairs up to its max HP.  Otherwise,
     * repairs the unit up to no maximum. */
    inline void repair(HitPoints amount, bool superhealth) {
      topLayer->repair(amount, superhealth);
    }
    /** \brief Change the type of the unit.
     *
     * The semantics of this are going to need some thought, but does
     * transforming of units, deployment and conversion to corpses on death. */
    inline void changeType(
        const UnitTypeId& to,
        hitPointAlteration hpAlteration
      ) { topLayer->changeType(to, hpAlteration); }
    inline void changeOwner(const PlayerId to, enum changeOwnerReason why) {
      topLayer->changeOwner(to, why);
      owner = topLayer->getOwner();
    }
    /** \brief Callback on unit destruction.
     *
     * This is passed through the UnitLayers, so that they can take appropriate
     * action.  This doesn't mean that the unit will be removed entirely; it
     * may merely turn into a corpse.  onRemoval is called when the unit is
     * really vanishing entirely. */
    inline void onDestruct(void) { topLayer->onDestruct(); }
    /** \brief Callback on unit removal.
     *
     * This is passed through the UnitLayers when the unit is going to be
     * entirely removed frm the game (rather than simply killed). */
    inline void onRemoval() { topLayer->onRemoval(); }
};

inline bool operator==(const LayeredUnit& left, const LayeredUnit& right) {
  return &left == &right;
}

}}

#endif // LIBSAKUSEN_SERVER__LAYEREDUNIT_H

