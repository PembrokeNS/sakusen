#include "updatedunit.h"

namespace sakusen {
namespace client {

void UpdatedUnit::incrementState()
{
  status.position += status.velocity;
}

}}

