#ifndef TEDOMARI_GAME__SMARTUNIT_H
#define TEDOMARI_GAME__SMARTUNIT_H

namespace tedomari {
namespace game {

class SmartUnit : public sakusen::client::UpdatedUnit {
  public:
    SmartUnit(const CompleteUnit& unit) :
      UpdatedUnit(unit)
    {}
};

}}

#endif // TEDOMARI_GAME__SMARTUNIT_H

