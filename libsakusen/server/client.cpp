#include "client.h"

using namespace sakusen::server;

Client::Client(ClientID i) :
  clientId(i),
  playerId(0),
  orderMessageQueue(),
  observer(false)
{
}

