#ifndef LIBSAKUSEN__COMPLETEUNIT_H
#define LIBSAKUSEN__COMPLETEUNIT_H

#include <sakusen/global.h>

#include <list>
#include <sakusen/point.h>
#include <sakusen/unittype.h>
#include <sakusen/unitstatus.h>
#include <sakusen/icompleteunit.h>

namespace sakusen {

/** \brief Bare implementation of ICompleteUnit
 *
 * This is simply a composition of an id, a UnitStatus and a UnitTypeData, thus
 * providing a simple implementation of ICompleteUnit with no extra bells or
 * whistles.
 *
 * See \ref units for more details about the various unit classes.
 */
class LIBSAKUSEN_API CompleteUnit : public ICompleteUnit {
  public:
    typedef boost::shared_ptr<CompleteUnit> Ptr;
    typedef boost::shared_ptr<const CompleteUnit> ConstPtr;
  private:
    CompleteUnit(); /**< Default constructor should not be used */
    CompleteUnit(
        UnitId uI,
        const UnitStatus& st,
        const UnitTypeData& tD
      );
  public:
    CompleteUnit(const Ref<const ICompleteUnit>& copy);
    virtual ~CompleteUnit() {}
  protected:
    UnitId unitId;
    UnitStatus status;

    /* modifiable stuff from UnitType */
    UnitTypeData typeData;
  public:
    /* accessors */
    inline UnitId getId(void) const {return unitId;}
    inline UnitStatus& getStatus(void) { return status; }
    inline const UnitStatus& getStatus(void) const { return status; }
    inline const IUnitStatus& getIStatus(void) const { return status; }
    inline const UnitTypeData& getTypeData(void) const { return typeData; }
    inline const IUnitTypeData& getITypeData(void) const { return typeData; }

    void store(OArchive&, bool sanitize = false) const;
    static CompleteUnit load(IArchive&, const DeserializationContext&);
};

}

#endif // LIBSAKUSEN__COMPLETEUNIT_H

