#ifndef LIBSAKUSEN__SENSORRETURN_H
#define LIBSAKUSEN__SENSORRETURN_H

#include <sakusen/global.h>
#include <sakusen/perception.h>
#include <sakusen/oarchive.h>
#include <sakusen/iarchive.h>

namespace sakusen {

/** \brief Most specific sensor return class
 *
 * A SensorReturn carries information about a particular return from a
 * particular target to a particular sensor of a particular unit.  It indicates
 * how well that senser can observe that unit.
 */
struct LIBSAKUSEN_API SensorReturn {
  public:
    SensorReturn() :
      perception(perception_none)
    {}
    SensorReturn(Perception p) :
      perception(p)
    {}

    Perception perception;

    /** \brief Reset the SensorReturn to indicate that it carries no
     * information. */
    inline void clear() {
      perception = perception_none;
    }

    void store(OArchive&) const;
    static SensorReturn load(IArchive&);
};

}

#endif // LIBSAKUSEN__SENSORRETURN_H

