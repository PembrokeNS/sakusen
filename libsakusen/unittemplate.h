#ifndef UNITTEMPLATE_H
#define UNITTEMPLATE_H

#include "libsakusen-global.h"

#include "point.h"
#include "unittype.h"
#include "orientation.h"
#include "universe.h"
#include "unitstatus.h"

namespace sakusen {

class LIBSAKUSEN_API UnitTemplate {
  private:
    UnitTemplate();
  public:
    UnitTemplate(
        const Universe* universe,
        const UnitStatus& status
      );
  private:
    const Universe* universe;
      /* a 'private line' to the universe is needed because there's no world
       * when UnitTemplates are being used */
    UnitStatus status;
    /* FIXME: subunits are not handled yet when this template is used to
     * construct a Map */

  public:
    const IUnitStatus* getStatus(void) const { return &status; }
    
    typedef Universe loadArgument;
    void store(OArchive&) const;
    static UnitTemplate load(IArchive&, const loadArgument*);
};
}

#endif // UNITTEMPLATE_H

