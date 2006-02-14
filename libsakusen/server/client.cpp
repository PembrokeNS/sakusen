#include "client.h"

using namespace sakusen::server;

Client::Client() :
  playerId(0),
  orderMessageQueue(),
  observer(false)
{
}

