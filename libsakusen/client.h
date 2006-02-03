#ifndef CLIENT_H
#define CLIENT_H

#include "libsakusen-global.h"

#include <queue>
#include "playerid.h"
#include "ordermessage.h"
#include "update.h"

namespace sakusen {

class LIBSAKUSEN_API Client {
  private:
    Client(const Client& copy);
  protected:
    /** Client is an abstract class.  All constructors must be protected. */
    Client();
  public:
    /** The default destructor does nothing. */
    virtual ~Client() {}
  protected:
    PlayerID playerId; /**< ID of player for this client.  "Dummy" clients for
                          atmospherics, etc. will have id 0 - the neutral
                          player */
    std::queue<OrderMessage> orderMessageQueue;
    bool observer;
  public:
    /* accessors */
    inline PlayerID getPlayerId() const { return playerId; }
    inline void setPlayerId(const PlayerID& id) { playerId = id; }
    inline bool orderMessageQueueEmpty(void) const {
      return orderMessageQueue.empty();
    }
    inline OrderMessage orderMessageQueuePopFront(void) {
      OrderMessage message = orderMessageQueue.front();
      orderMessageQueue.pop();
      return message;
    }
    inline bool isObserver() const { return observer; }

    /** interface which must be implemented by sublasses */
    virtual void sendUpdate(const Update& update) = 0;
};

#ifdef NEED_TEMPLATE_INSTANTIATION
#include <list>
#pragma warning (disable: 4231)
EXPORT_LIST(Client*)
#endif

}

#endif // CLIENT_H

