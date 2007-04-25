#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "libsakusen-global.h"

#include <vector>
#include <boost/utility.hpp>

#include "gnu_extensions.h"
#include "stringutils.h"
#include "unittype.h"
#include "iarchive.h"
#include "oarchive.h"

namespace sakusen {

/** \brief Encapsulates information about the universe in which a game is
 * played
 *
 * This class contains the information you need before making a Map - things
 * such as what UnitTypes, WeaponTypes, etc exist.
 */
class LIBSAKUSEN_API Universe : boost::noncopyable {
  public:
    typedef boost::shared_ptr<Universe> Ptr;
    typedef boost::shared_ptr<const Universe> ConstPtr;

    Universe(
        const String& internalName,
        const String& hash,
        const std::vector<WeaponType>& weaponTypes,
        const std::vector<UnitType>& unitTypes
      );
    ~Universe() {}
  private:
    String internalName;
    String hash;
    std::vector<WeaponType> weaponTypes;
    hash_map_string<WeaponTypeID>::type weaponIDLookup;
    std::vector<UnitType> unitTypes;
    hash_map_string<UnitTypeID>::type unitIDLookup;

    void constructHashMaps();
  public:
    /** \brief finishes initialization of Universe
     *
     * This method finishes off the initialization of the Universe by resolving
     * all the names that have been inserted into it into IDs.
     *
     * If an unresolvable name is encoutered then it is returned (in this case
     * the Universe should *not* be used for anything).  Otherwise,
     * the null String is returned.
     */
    String resolveNames();
    
    /* accessors */
    inline const String& getInternalName() const { return internalName; }
    inline const String& getHash() const { return hash; }
    
    /* index/pointer/ID/Name conversions */
    inline const WeaponType* getWeaponTypePtr(WeaponTypeID id) const
    {
      return id;
    }
    WeaponTypeID getWeaponTypeID(String weaponTypeName) const;
    inline const UnitTypeID getUnitTypeId(uint32 i) const
    {
      assert(i<unitTypes.size());
      return &unitTypes[i];
    }
    inline const UnitType* getUnitTypePtr(const UnitTypeID id) const
    {
      return id;
    }
    inline const UnitType* getUnitTypePtr(uint32 i) const
    {
      return getUnitTypePtr(getUnitTypeId(i));
    }
    UnitTypeID getUnitTypeId(String unitTypeName) const;
    
    bool containsUnitType(const UnitTypeID id) const;

    typedef void loadArgument;
    void store(OArchive&) const;
    /** \brief Loads a Universe from an archive
     *
     * This method attempts to create a Universe from the contents of the given
     * archive, as saved with Universe::store.
     *
     * It returns a newly allocated Universe, which must be deleted by the
     * caller.
     */
    static Universe* loadNew(IArchive&, ResourceInterface::Ptr);
};

}

#endif // UNIVERSE_H

