#ifndef UNITLAYER_H
#define UNITLAYER_H

#include "hitpointalteration.h"
#include "playerid.h"
#include "changeownerreason.h"
#include "unitstatus.h"

namespace sakusen {
namespace server {

class LayeredUnit;
class UnitCore;
class UnitMask;

/** \brief Base class for all layers on LayeredUnits
 *
 * Essentially an interface specifying what methods should be implemented by
 * any layer of a LayeredUnit.
 */
class UnitLayer : public IUnitTypeData {
  public:
    typedef boost::shared_ptr<UnitLayer> Ptr;
    typedef boost::shared_ptr<const UnitLayer> ConstPtr;
  protected:
    UnitLayer() {}
    UnitLayer(const UnitLayer& copy) : IUnitTypeData(copy) {}
  public:
    virtual ~UnitLayer() {}

    /* methods to do with the layering */
    virtual UnitLayer::Ptr newCopy(LayeredUnit* outer) const = 0;
    virtual UnitCore* getCore() = 0;
    virtual boost::shared_ptr<UnitLayer> getLayer(const std::type_info&) = 0;
    virtual void removeLayer(const UnitMask*) = 0;
    virtual Ref<LayeredUnit> getOuterUnit() = 0;
    virtual Ref<const LayeredUnit> getOuterUnit() const = 0;

    /* accessors (more accessors are inherited from IUnitTypeData) */
    virtual PlayerID getOwner() const = 0;

    /* game mechanics */
    virtual void incrementWeaponsState() = 0;
    virtual bool kill(HitPoints excessDamage) = 0;
    virtual void damage(HitPoints amount) = 0;
    virtual void repair(HitPoints amount, bool superhealth) = 0;
    virtual void changeType(
        const UnitTypeID& to,
        hitPointAlteration hpAlteration
      ) = 0;
    virtual void changeOwner(const PlayerID to, enum changeOwnerReason why) = 0;

    /* callbacks */
    virtual void onCreate(void) = 0;
    virtual void onDestruct(void) = 0;
};

}}

#endif // UNITLAYER_H

