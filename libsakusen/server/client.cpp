#include "client.h"

using namespace sakusen::server;

Client::Client(ClientId i) :
  clientId(i),
  playerId(0),
  orderMessageQueue(),
  observer(false)
{
}

