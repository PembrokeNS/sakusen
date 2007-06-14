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
class UnitLayer : public IUnitTypeData, boost::noncopyable {
  public:
    typedef boost::shared_ptr<UnitLayer> Ptr;
    typedef boost::shared_ptr<const UnitLayer> ConstPtr;
  protected:
    UnitLayer() {}
  public:
    virtual ~UnitLayer() {}

    /* methods to do with the layering */
    /** \brief Get the UnitCore at the bottom of the sequence of layers of
     * which this is a part. */
    virtual UnitCore* getCore() = 0;
    /** \brief Search for a layer of a given type.
     *
     * Examines the layers below this one (but not this one itself) for the
     * highest whose type matches the given type_info.  If one is found, it is
     * returned.  If none is found then a null pointer is returned. */
    virtual boost::shared_ptr<UnitLayer> getLayer(const std::type_info&) = 0;
    /** \brief Remove a layer from the sequence.
     *
     * Removes given layer from the sequence.  Fatals if the layer is not
     * found. */
    virtual void removeLayer(const UnitMask*) = 0;
    /** \brief Get LayeredUnit of which this layer is a part. */
    virtual Ref<LayeredUnit> getOuterUnit() = 0;
    /** \brief Get LayeredUnit of which this layer is a part. */
    virtual Ref<const LayeredUnit> getOuterUnit() const = 0;

    /* accessors (more accessors are inherited from IUnitTypeData) */
    virtual PlayerId getOwner() const = 0;

    /** \name Game mechanics.
     *
     * See similarly named methods in LayeredUnit for more details. */
    //@{
    virtual void incrementWeaponsState() = 0;
    virtual bool kill(HitPoints excessDamage) = 0;
    virtual void damage(HitPoints amount) = 0;
    virtual void repair(HitPoints amount, bool superhealth) = 0;
    virtual void changeType(
        const UnitTypeId& to,
        hitPointAlteration hpAlteration
      ) = 0;
    virtual void changeOwner(const PlayerId to, enum changeOwnerReason why) = 0;
    //@}

    /* callbacks */
    virtual void onCreate() = 0;
    virtual void onDestruct() = 0;
    virtual void onRemoval() = 0;
};

}}

#endif // UNITLAYER_H

