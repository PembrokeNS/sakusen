#ifndef UPDATEDUNIT_H
#define UPDATEDUNIT_H

#include "libsakusen-client-global.h"

#include "completeunit.h"
#include "updatedata.h"
#include "unitorders.h"

namespace sakusen {
namespace client {

/** \brief A player-owned Unit that can accept information about its orders
 * and status when they arrive over the network.
 */
class LIBSAKUSEN_CLIENT_API UpdatedUnit : public CompleteUnit {
  public:
    typedef boost::shared_ptr<UpdatedUnit> Ptr;
    typedef boost::shared_ptr<const UpdatedUnit> ConstPtr;
  private:
    UpdatedUnit();
  protected:
    UpdatedUnit(const CompleteUnit& copy) :
      CompleteUnit(copy),
      orders(static_cast<uint16>(copy.getStatus().getWeaponsStatus().size())),
      altered(false)
    {}
  public:
    virtual inline ~UpdatedUnit() = 0;
  private:
    /** This Unit's current order status. */
    UnitOrders orders;
    /** Whether this Unit has already been altered this tick */
    bool altered;
  public:
    /** \name Accessors */
    /*@{*/
    inline const UnitOrders &getUnitOrders() const {
      return orders;
    }
    inline bool getAltered() const {
      return altered;
    }
    /*@}*/
    virtual void orderAccepted(const OrderAcceptedUpdateData& data);
    void alter(const CompleteUnit&);
    virtual void incrementState();
    virtual void destroying() {}
};

UpdatedUnit::~UpdatedUnit() {
}

/** \brief function object to extract the id from an UpdatedUnit */
struct UpdatedUnitIder {
  UnitId operator()(const Ref<UpdatedUnit>& u) const {
    return u->getId();
  }
};

}}

#endif // UPDATEDUNIT_H

