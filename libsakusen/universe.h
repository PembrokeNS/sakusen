#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "libsakusen-global.h"

#include <vector>

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
class LIBSAKUSEN_API Universe {
  private:
    Universe();
    /** \brief Explicitly private copy constructor to prevent copying
     *
     * Because UnitTypeIDs, etc. are actually pointers to objects stored in
     * Universe, a Universe cannot be copied (it would invalidate those
     * pointers).
     */
    Universe(const Universe&);
  public:
    Universe(
        const String& internalName,
        const std::vector<UnitType>& unitTypes
      );
    ~Universe() {}
  private:
    String internalName;
    std::vector<UnitType> unitTypes;
    __gnu_cxx::hash_map<String, UnitTypeID, StringHash> unitIDLookup;

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
    
    /* index/pointer/ID/Name conversions */
    inline const WeaponType* getWeaponTypePtr(const WeaponTypeID id) const
    {
      return id;
    }
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
    WeaponTypeID getWeaponTypeID(String weaponTypeName) const;
    UnitTypeID getUnitTypeID(String unitTypeName) const;
    
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
    static Universe* loadNew(IArchive&);
};

}

#endif // UNIVERSE_H

