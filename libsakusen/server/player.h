#ifndef LIBSAKUSEN_SERVER__PLAYER_H
#define LIBSAKUSEN_SERVER__PLAYER_H

#include "libsakusen-global.h"
#include "hash_list.h"
#include "gnu_extensions.h"
#include "playerid.h"
#include "client.h"
#include "updatetype.h"
#include "changeownerreason.h"
#include "playertemplate.h"
#include "maskedptr.h"
#include "idindex.h"
#include "dynamicsensorreturns.h"
#include "ballistic.h"

namespace sakusen {
namespace server {

class LayeredUnit;

class LIBSAKUSEN_SERVER_API Player : public MaterielProvider {
  private:
    Player();
  public:
    Player(const PlayerTemplate& t, const PlayerId id);
    Player(const Player& copy);
    Player& operator=(const Player&);
    ~Player();

    typedef IdIndex<
        SensorReturnsId, DynamicSensorReturns, DynamicSensorReturnsIder
      > SensorReturnsIdIndex;
  private:
    bool noClients;
    bool raceFixed;
    PlayerId playerId;
    String name;
    std::list<Client*> clients;
    
    /* This is a hashtable of the units belonging to the player, keyed by
       their id */
    __gnu_cxx::hash_map<UnitId, Ref<LayeredUnit> > units;
    /* The id of the next unit to be added for this player */
    UnitId nextUnitId;
    
    hash_list<DynamicSensorReturns, DynamicSensorReturns> sensorReturns;
    /* Index into sensorReturns giving access by Id */
    SensorReturnsIdIndex::Ptr sensorReturnsById;
    SensorReturnsId nextSensorReturnsId;

    /* A container of Refs to the Ballistics visible to this player.  The key
     * is a pointer which we use as a UID for the Ballistic, but it shouldn't
     * be dereferenced - use the Ref instead to avoid danglingness.  The first
     * pair entry is the client-side id */
    __gnu_cxx::hash_map<
        MaskedPtr<Ballistic>,
        std::pair<ClientBallisticId, Ref<const Ballistic> >
      > visibleBallistics;
    ClientBallisticId nextClientBallisticId;

    uint64 availableEnergy;
    uint64 availableMetal;
  public:
    /** \name Accessors. */
    //@{
    PlayerId getId() const { return playerId; }
    inline const String& getName(void) const { return name; }
    inline void setName(const String& n) { name = n; }
    inline uint16 getNumClients(void) const {
      return static_cast<uint16>(clients.size());
    }
    void attachClient(Client* client);
    void detachClient(Client* client);
    const __gnu_cxx::hash_map<UnitId, Ref<LayeredUnit> >& getUnits(void) const {
      return units;
    }
    Ref<DynamicSensorReturns> getSensorReturns(SensorReturnsId id) {
      return sensorReturnsById->find(id);
    }
    uint64 getAvailableEnergy() const { return availableEnergy; }
    uint64 getAvailableMetal() const { return availableMetal; }

    inline bool isReadyForGameStart(void) const {
      /** \bug Should check that a race is selected. */
      return (clients.size() > 0 || noClients);
    }
    //@}
    
    /** \name Materiel request functions
     *
     * Implementation of MaterielProvider interface. */
    //@{
    virtual uint32 requestEnergy(uint32 amount);
    virtual uint32 requestMetal(uint32 amount);
    //@}

    /* game mechanics */
    void removeUnit(const UnitId id, enum changeOwnerReason why);
    void addUnit(const Ref<LayeredUnit>& unit, enum changeOwnerReason why);
    void checkSensorReturns();
    
    void applyIncomingOrders(void);
    
    void informClients(const Update& update);
};

}}

#include "layeredunit.h"

#endif // LIBSAKUSEN_SERVER__PLAYER_H

