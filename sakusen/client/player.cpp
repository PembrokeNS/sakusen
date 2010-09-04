#include <sakusen/client/player.h>
#include <sakusen/updatedata.h>

using namespace std;

using namespace sakusen::client;

Player::Player(const Player& copy) :
  playerId(copy.playerId),
  name(copy.name),
  clients()
{
  for (list<Client*>::const_iterator client = copy.clients.begin();
      client != copy.clients.end(); ++client) {
    clients.push_back((*client)->newCopy());
  }
}

Player::~Player()
{
  while (!clients.empty()) {
    delete clients.front();
    clients.pop_front();
  }
}

