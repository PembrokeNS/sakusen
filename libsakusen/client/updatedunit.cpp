#include "updatedunit.h"

namespace sakusen {
namespace client {

void UpdatedUnit::incrementState()
{
  if (updated) {
    updated = false;
  } else {
    status.position += status.velocity;
  }
}

}}

