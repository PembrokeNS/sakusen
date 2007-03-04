#ifndef CLIENT_H
#define CLIENT_H

#include "libsakusen-global.h"
#include "libsakusen-server-global.h"

#include <queue>
#include "playerid.h"
#include "clientid.h"
#include "ordermessage.h"
#include "update.h"

namespace sakusen {
namespace server {

class LIBSAKUSEN_SERVER_API Client : boost::noncopyable {
  private:
    Client(const Client& copy);
  protected:
    /** Client is an abstract class.  All constructors must be protected. */
    Client(ClientID id);
  public:
    /** The default destructor does nothing. */
    virtual ~Client() {}
  protected:
    const ClientID clientId;
    PlayerID playerId; /**< ID of player for this client.  "Dummy" clients for
                          atmospherics, etc. will have id 0 - the neutral
                          player */
    std::queue<OrderMessage> orderMessageQueue;
    bool observer;
  public:
    /** \name Accessors */
    //@{
    inline ClientID getClientId() const { return clientId; }
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
    //@}

    /** \name Interface which must be implemented by subclasses */
    //@{
    /** \brief Queue an Update to be sent to the client */
    virtual void queueUpdate(const Update& update) = 0;
    /** \brief Forward all queued Updates to the client
     *
     * \param time The game time to which the dispatched Updates correspond */
    virtual void flushOutgoing(Time time) = 0;
    //@}
};

}}

#endif // CLIENT_H

