#ifndef UNITTEMPLATE_H
#define UNITTEMPLATE_H

#include "libsakusen-global.h"

#include "point.h"
#include "unittype.h"
#include "orientation.h"
#include "universe.h"
#include "unitstatus.h"

namespace sakusen {

/** \brief Defines the initial state of a unit at the start of play.
 *
 * \bug Subunits are not handled yet. */
class LIBSAKUSEN_API UnitTemplate {
  private:
    UnitTemplate();
  public:
    UnitTemplate(
        const Universe::ConstPtr& universe,
        const UnitStatus& status
      );
  private:
    /** \brief Universe containing type for this tempalte.
     *
     * A 'private line' to the universe is needed because there's no world
     * when UnitTemplates are being used. */
    Universe::ConstPtr universe;
    
    /** \brief Initial state of the unit */
    UnitStatus status;
  public:
    const IUnitStatus& getStatus(void) const { return status; }
    
    void store(OArchive&) const;
    static UnitTemplate load(IArchive&, const DeserializationContext&);
};
}

#endif // UNITTEMPLATE_H

