#include "client.h"

using namespace sakusen;

Client::Client() :
  playerId(0),
  orderMessageQueue(),
  observer(false)
{
}

