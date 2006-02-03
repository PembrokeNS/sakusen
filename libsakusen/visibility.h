#ifndef VISIBILITY_H
#define VISIBILITY_H

#include "libsakusen-global.h"
#include "angle.h"
#include "iarchive.h"
#include "oarchive.h"

namespace sakusen {

/** \file
 * This file holds various sensor-related things. They are all in the
 * same file because it is expected that you will probably not want to use only
 * one of them in any given place -- they all interdepend quite heavily.
 *
 * A visibility of 255 (the largest uint8) for any sensor means the object is
 * always visible to that sensor (but see the todo below). A Visibility of 0
 * means the object is never visible to that sensor. Similarly, for
 * SensorCapability, a range of 0 means the object can detect only objects of
 * Visibility 255.  If you wish the object not to be able to detect even Visibility
 * 255 objects, then you must clear \c capable.  A range of the
 * largest uint32 means the object can see everything on that sensor except
 * things of Visibility 0.
 * 
 * Visibilities of 0 and capabilities of -1 should only be used for
 * special reasons by level designers.
 * 
 * \todo It is not yet decided whether max. visibility is affected by
 * environment. Does an object with max. seismar visibility only appear if it is
 * on the ground, and only to other ground-based objects? Does an object with
 * max. sonar visibility only appear if it is in the water, and only to other
 * water-borne objects? If not, we may need to reserve 254 to mean almost-always
 * visible.
 *
 * \warning Just because you do not expect an object to ever be underwater does
 * not mean you should set its sonar visibility to 0. Set it to a sensible value
 * just in case.
 */

/** \brief describes a type of sensor
 * 
 * This enumeration holds a type of sensor. It is expected to be used where a
 * sensor has to be described, e.g. in a ::SensorReturn to describe by what
 * sensor an object was detected.
 */
enum sensorType {
  optical,
  infraRed,
  radarPassive,
  radarActive,
  sonarPassive,
  sonarActive,
  seismar
};

/** \brief describes an object's visibility to each type of sensor 
 *
 * This class is just a collection of uint8's, each describing how visible the
 * appropriate object is to a type of sensor. For two objects A and B, A's
 * ::Sensors, B's ::Visiblity, the distance between A and B, the topography of
 * the area and the local atmospheric conditions all contribute to how reliably
 * (if at all) A can detect B.
 *
 * As this class has no internal state all its values are public and you will
 * probably just construct it and set the non-zero ones however you like.
 *
 * \code
 * Visiblity *v;
 * v = new Visibility();
 * v->optical = 20;
 * \endcode
 *
 */
class LIBSAKUSEN_API Visibility {
  public:
    /** default constructor -- zeroes all values */
    Visibility();
    /** constructs by extracting data from archive */
    Visibility(IArchive&);
    /** \brief does nothing for now
     *
     * The destructor at present does nothing but this is not guaranteed for
     * future versions.
     */
    ~Visibility() {}
    uint8 optical;
    uint8 infraRed;
    uint8 radar;
    uint8 passiveRadar;
    uint8 sonar;
    uint8 passiveSonar;
    uint8 seismar;
    void store(OArchive& archive) const {
      archive << optical << infraRed << radar << passiveRadar << sonar <<
        passiveSonar << seismar;
    }
};

/** \brief Describes capability with one sensor.
 * 
 * This struct contains all the information related to the use of any one
 * sensor.
 */
struct SensorCapability {
  SensorCapability() :
    capable(false), range(0), falloff(0) {}
  SensorCapability(IArchive& archive) {
    archive >> capable >> range >> falloff;
  }
  
  bool capable; /**< false iff unit doesn't have this sensor */
  uint32 range; /**< corresponds to distances in the world */
  Angle falloff; /**< describes the rate of falloff from looking straight
                   ahead */
  inline void store(OArchive& archive) const {
    archive << capable << range << falloff;
  }
};

/** \brief Describes an object's overall sensor capabilities.
 *
 * This struct contains a ::SensorCapability for each sensor type, thus contains
 * all the information related to any one object's sensors.
 */
struct Sensors {
  Sensors() :
    optical(),
    infraRed(),
    seismar(),
    radarPassive(),
    radarActive(),
    sonarPassive(),
    sonarActive()
  {}
  Sensors(IArchive& archive) :
    optical(archive),
    infraRed(archive),
    seismar(archive),
    radarPassive(archive),
    radarActive(archive),
    sonarPassive(archive),
    sonarActive(archive)
  {}
  
  struct SensorCapability optical;
  struct SensorCapability infraRed;
  struct SensorCapability seismar;
  struct SensorCapability radarPassive;
  struct SensorCapability radarActive;
  struct SensorCapability sonarPassive;
  struct SensorCapability sonarActive;
  
  void store(OArchive& archive) const {
    optical.store(archive);
    infraRed.store(archive);
    seismar.store(archive);
    radarPassive.store(archive);
    radarActive.store(archive);
    sonarPassive.store(archive);
    sonarActive.store(archive);
  }
};

}

#endif
