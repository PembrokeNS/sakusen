#ifndef TEDOMARI__GAME__SMARTUNIT_H
#define TEDOMARI__GAME__SMARTUNIT_H

namespace tedomari {
namespace game {

class SmartUnit : public sakusen::client::UpdatedUnit {
  public:
    SmartUnit(const CompleteUnit& unit) :
      UpdatedUnit(unit)
    {}
};

}}

#endif // TEDOMARI__GAME__SMARTUNIT_H

