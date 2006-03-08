#ifndef UPDATEDUNIT_H
#define UPDATEDUNIT_H

#include "completeunit.h"

namespace sakusen {
namespace client {

class UpdatedUnit : public CompleteUnit {
  private:
    UpdatedUnit();
  public:
    UpdatedUnit(const CompleteUnit& copy) :
      CompleteUnit(copy), updated(false)
    {}
    virtual ~UpdatedUnit() {}
  private:
    /** Whether this Unit has already been updated this tick */
    bool updated;
  public:
    void incrementState();
};

}}

#endif // UPDATEDUNIT_H

