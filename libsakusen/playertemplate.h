#ifndef PLAYERTEMPLATE_H
#define PLAYERTEMPLATE_H

#include <vector>

#include "unittemplate.h"

namespace sakusen {

class MapTemplate;

class LIBSAKUSEN_API PlayerTemplate {
  private:
    PlayerTemplate();
  public:
    PlayerTemplate(
        bool noClients,
        bool fixedRace,
        const std::vector<UnitTemplate>& units
      );
  private:
    bool noClients;
      /**< indicates that a player is not supposed to be controlled by
       * clients (including AI clients) */
    bool raceFixed;
      /**< indicates that there is no choice as to the race of a player */
    std::vector<UnitTemplate> units;
      /**< provides a list of all the units that a player starts with */
  public:
    inline bool isNoClients() const { return noClients; }
    inline bool isRaceFixed() const { return raceFixed; }
    inline const std::vector<UnitTemplate>& getUnits() const { return units; }
    bool sanityCheck(const Universe* universe, const MapTemplate& map);
      /* Confirms that all specified units exist in the given universe, and
       * that they all fit onto the given map.  Returns true if a
       * problem is found */

    typedef Universe loadArgument;
    void store(OArchive&) const;
    static PlayerTemplate load(IArchive&, const loadArgument*);
};

}

#endif // PLAYERTEMPLATE_H

