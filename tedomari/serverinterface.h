#ifndef SERVERINTERFACE_H
#define SERVERINTERFACE_H

#include "libsakusen-comms-global.h"
#include "tedomari-global.h"
#include "timeutils.h"
#include "socket.h"
#include "game/game.h"

namespace tedomari {

/** \brief Serves as a wrapper for communication with the server
 *
 * This class handles the Socket objects used for communication with the
 * server, provides methods for sending various types of message to the server,
 * and filters and forwards incoming messages to appropriate destinations. */
class ServerInterface {
  private:
    ServerInterface();
    ServerInterface(const ServerInterface& copy);
  public:
    /** \brief Standard constructor
     *
     * \param joinAddress Address for joining the server.
     * \param unixSockets True iff unix sockets should be used where possible.
     * \param abstract True iff the abstract unix socket namespace should be
     * used where possible.
     * \param game Game object to which updates about the game state should be
     * sent. */
    ServerInterface(
        const String& joinAddress,
#ifndef DISABLE_UNIX_SOCKETS
        bool unixSockets,
        bool abstract,
#endif
        game::Game* game
      );
    ~ServerInterface();
  private:
    /** Address to which join requests should be sent. */
    String joinAddress;
    game::Game* game; /**< not owned by this */
    struct timeval timeout;
#ifndef DISABLE_UNIX_SOCKETS
    bool unixSockets; /**< OK to use Unix sockets */
    bool abstract; /**< OK to use abstract namespace */
#endif
    /** true iff we have joined the game */
    bool joined;
    /** Id assigned to us by the server (valid only when joined) */
    sakusen::ClientId id;
    /** Socket used by the server to talk to us (owned by this) */
    sakusen::comms::Socket::Ptr incomingSocket;
    /** Socket the server has reserved for us to talk to it (owned by this,
     * but might be the same as incomingSocket, so don't delete both before
     * checking that) */
    sakusen::comms::Socket::Ptr outgoingSocket;

    String universeName;

    void initialSettingsSetup();
    void settingAlteration(const String& setting, const String& value);
  public:
    /** \name Accessors */
    //@{
    inline bool isJoined() const { return joined; }
    inline sakusen::ClientId getId() const { return id; }
    //@}
    
    String flushIncoming(const sakusen::ResourceInterface::Ptr&);
    String join();
    bool leave(bool sendMessage);
    bool send(const sakusen::comms::MessageData* message);
    bool getSetting(const String& setting);
    bool setSetting(const String& setting, const String& value);
    bool getClientSetting(const String& setting);
    bool setClientSetting(const String& setting, const String& value);
};

}

#endif // SERVERINTERFACE_H

