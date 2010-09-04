#include "client.h"

using namespace sakusen;
using namespace sakusen::server;

Client::Client(ClientId i) :
  clientId(i),
  playerId(),
  orderMessageQueue(),
  observer(false)
{
}

