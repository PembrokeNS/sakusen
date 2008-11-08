#ifndef LIBSAKUSEN_SERVER__CLIENT_H
#define LIBSAKUSEN_SERVER__CLIENT_H

#include "libsakusen-global.h"
#include "libsakusen-server-global.h"

#include <queue>
#include "playerid.h"
#include "clientid.h"
#include "ordermessage.h"
#include "update.h"

namespace sakusen {
namespace server {

/** \brief Represents a client from the perspective of the server.
 *
 * This class handles the communication between the server and one particular
 * client, providing for sending updates and buffering incoming orders.
 *
 * This is an abstract class, and does not assume any particular communication
 * channel to the client.  Subclasses will implement some specific means, such
 * as network communication.
 */
class LIBSAKUSEN_SERVER_API Client : private boost::noncopyable {
  private:
    Client(const Client& copy);
  protected:
    /** Client is an abstract class.  All constructors must be protected. */
    Client(ClientId id);
  public:
    /** The default destructor does nothing. */
    virtual ~Client() {}
  protected:
    const ClientId clientId;
    PlayerId playerId; /**< Id of player for this client.  "Dummy" clients for
                          atmospherics, etc. will have id 0 - the neutral
                          player */
    /** \brief Buffer for incoming orders from clients.
     *
     * This queue is filled in some manner which depends on the implementation
     * of the class.  At the appropriate time, the orders can be extracted by
     * calling orderMessageQueuePopFront.
     */
    std::queue<OrderMessage> orderMessageQueue;
    bool observer;
  public:
    /** \name Accessors */
    //@{
    inline ClientId getClientId() const { return clientId; }
    inline PlayerId getPlayerId() const { return playerId; }
    inline void setPlayerId(const PlayerId& id) { playerId = id; }
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

#endif // LIBSAKUSEN_SERVER__CLIENT_H

