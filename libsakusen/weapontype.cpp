#include "weapontype.h"

using namespace sakusen;

WeaponType::WeaponType(
    String iN,
    String mN,
    uint16 eC,
    uint16 mC,
    uint16 eR,
    uint16 mR,
    ResourceInterface* resourceInterface
  ) :
  internalName(iN),
  moduleName(mN),
  spawnFunction(NULL),
  energyCost(eC),
  metalCost(mC),
  energyRate(eR),
  metalRate(mR)
{
  ResourceSearchResult result;
  spawnFunction = resourceInterface->
    symbolSearch<server::Weapon* (*)(const WeaponType*)>(
        moduleName, "spawn_"+internalName, &result
      );
  switch (result)
  {
    case resourceSearchResult_notSupported:
      /* This means we are a client and don't need the thing anyway; we
       * paranoically assign NULL again. */
      spawnFunction = NULL;
      return;
    case resourceSearchResult_success:
      /* Everything is OK */
      return;
    default:
      /* There is a problem */
      throw ResourceDeserializationExn(
          moduleName, result, resourceInterface->getError()
        );
  }
}

/** \brief Spawn a weapon of this type which must be deleted by the caller */
server::Weapon* WeaponType::spawn() const
{
  if (spawnFunction == NULL) {
    Fatal("missing spawn function");
  }

  return (*spawnFunction)(this);
}

void WeaponType::store(OArchive& archive) const
{
  archive << internalName << moduleName << energyCost << metalCost <<
    energyRate << metalRate;
}

WeaponType WeaponType::load(
    IArchive& archive,
    ResourceInterface* resourceInterface
  )
{
  String internalName;
  String moduleName;
  uint16 energyCost;
  uint16 metalCost;
  uint16 energyRate;
  uint16 metalRate;
  archive >> internalName >> moduleName >> energyCost >> metalCost >>
    energyRate >> metalRate;
  return WeaponType(
      internalName, moduleName, energyCost, metalCost, energyRate, metalRate,
      resourceInterface
    );
}

