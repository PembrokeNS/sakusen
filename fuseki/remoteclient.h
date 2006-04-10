#ifndef REMOTECLIENT_H
#define REMOTECLIENT_H

#include <list>

#include "client.h"
#include "socket.h"
#include "settingsuser.h"
#include "settingstree/leaf.h"

namespace fuseki {

/** \brief Interfaces witha client via one or more Sockets
 *
 * Represents a client which is contacted via a socket of some kind (this could
 * be a TCP socket to another machine, or a Unix socket to another process on
 * the same machine, or whatever else is supported) */
class RemoteClient : public sakusen::server::Client, public SettingsUser {
  private:
    RemoteClient();
    RemoteClient(const RemoteClient& copy);
  public:
    /** \brief Standard constructor
     *
     * \param id The id to be assigned to this RemoteClient.
     * \param server The server with which this RemoteClient is associated.
     * \param socket The Socket to be used by the RemoteClient to send messages
     * to the client.  Ownership of \a socket is transferred to this object.
     * \param createInSocket Whether a seperate socjet should be created for
     * incoming communication from the client.  If false, \a socket will be
     * used for communication in both directions.
     * \param abstract (Only when unix sockets are enabled) If createInSocket
     * is set, and the socket to be created is a unix socket, then this
     * determines whether the abstract socket namespace should be used for that
     * socket. */
    RemoteClient(
        sakusen::comms::ClientID id,
        Server* server,
        sakusen::comms::Socket* socket,
        bool createInSocket
#ifndef DISABLE_UNIX_SOCKETS
        ,
        bool abstract
#endif
      );
    ~RemoteClient();
  private:
    sakusen::comms::ClientID id;
    Server* server;
    sakusen::comms::Socket* inSocket;
    sakusen::comms::Socket* outSocket;
    std::queue<sakusen::comms::Message, std::list<sakusen::comms::Message> > incomingMessageQueue;
    std::list<sakusen::Update> outgoingUpdateQueue;
    bool admin; /* Whether I am an admin */
    bool neverAdmin; /* Whether I never want to be an admin */
    bool ready; /* Whether I am ready to start the game */
    bool autoUnready; /* Whether I want to have my readiness cleared whenever
                         a setting changes */

    void setPlayerId(sakusen::PlayerID id, bool removeGroup);
    inline void setPlayerId(sakusen::PlayerID id) {
      setPlayerId(id, true);
    }
  public:
    /** \name Accessors */
    //@{
    inline sakusen::comms::ClientID getId() { return id; }
    inline bool isAdmin() { return admin; }
    inline void setAdmin(bool value);
    inline bool isNeverAdmin() { return neverAdmin; }
    inline bool isAutoUnready() { return autoUnready; }
    inline bool messageQueueEmpty() const {
      return incomingMessageQueue.empty();
    }
    inline sakusen::comms::Message messageQueuePopFront() {
      assert(!incomingMessageQueue.empty());
      sakusen::comms::Message message = incomingMessageQueue.front();
      incomingMessageQueue.pop();
      return message;
    }
    //@}
    
    /** \brief Adds an order to the back of the Client's order queue
     *
     * \param o OrderMessage to add */
    inline void enqueueOrder(const sakusen::OrderMessage& o) {
      orderMessageQueue.push(o);
    }

    /** \brief Returns true iff this Client is ready for the game to start */
    inline bool isReadyForGameStart() {
      return ready && (observer || (playerId != 0));
    }
    
    /** \brief Process all pending Messages on inSocket
     *
     * This receives as much data as posible on the incoming Socket and
     * enqueues the Messages it obtains in the message queue. */
    void flushIncoming();
    void flushOutgoing(sakusen::Time time);
    /** \brief Send the given message at once
     *
     * \param data MessageData to use to construct the message to be sent */
    inline void send(const sakusen::comms::MessageData& data) {
      outSocket->send(data);
    }
    inline void queueUpdate(const sakusen::Update& update) {
      outgoingUpdateQueue.push_back(update);
    }

    /** \name Perform magic related to the changing value of a client setting
     *
     * See \ref settingstree for more details
     * */
    //@{
    String performBoolMagic(
        const std::list<String>& name,
        bool newValue
      );
    template<typename T>
    String performIntMagic(
        const std::list<String>& name,
        T newValue
      );
    String performStringMagic(
        const std::list<String>& name,
        const String& newValue
      );
    String performStringSetMagic(
        const std::list<String>& name,
        const __gnu_cxx::hash_set<String, sakusen::StringHash>& newValue
      );
    //@}
};

}

#endif // REMOTECLIENT_H

