#include "unittemplate.h"

#include "world.h"

using namespace sakusen;

UnitTemplate::UnitTemplate(
    const Universe::ConstPtr& u,
    const UnitStatus& s
  ) :
  universe(u),
  status(s)
{
  assert(world==NULL); /* This constructor is for when making maps, not when
                          playing the game */
  assert (!status.radarIsActive || universe->getUnitTypePtr(
        status.type
      )->getDynamicData().getVision().radarActive.capable);
  assert (!status.sonarIsActive || universe->getUnitTypePtr(
        status.type
      )->getDynamicData().getVision().sonarActive.capable);
}

void UnitTemplate::store(OArchive& out) const
{
  out.magicValue<2>("UT");
  status.store(out, universe);
}

UnitTemplate UnitTemplate::load(
    IArchive& in,
    const Universe::ConstPtr* universe
  )
{
  in.magicValue<2>("UT");
  return UnitTemplate(*universe, UnitStatus::load(in, universe));
}

