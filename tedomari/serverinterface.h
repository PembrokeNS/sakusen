#ifndef SERVERINTERFACE_H
#define SERVERINTERFACE_H

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
     * \param solicitationSocket Socket to use to solicit the server.
     * \param joinAddress Address for joining the server.
     * \param unixSockets True iff unix sockets should be used where possible.
     * \param abstract True iff the abstract unix socket namespace should be
     * used where possible.
     * \param game Game object to which updates about the game state should be
     * sent. */
    ServerInterface(
        sakusen::comms::Socket* solicitationSocket,
        const String& joinAddress,
#ifndef DISABLE_UNIX_SOCKETS
        bool unixSockets,
        bool abstract,
#endif
        game::Game* game
      );
    ~ServerInterface();
  private:
    /** Socket for solicitation/advertising (owned by code that constructs
     * this object) */
    sakusen::comms::Socket* solicitationSocket;
    /** Address to which join requests should be sent, if different from the
     * solicitationSocket */
    String joinAddress;
    game::Game* game; /* not owned by this */
    struct timeval timeout;
#ifndef DISABLE_UNIX_SOCKETS
    bool unixSockets; /* OK to use Unix sockets */
    bool abstract; /* OK to use abstract namespace */
#endif
    bool joined;
    /** ID assigned to us by the server (valid only when joined) */
    sakusen::comms::ClientID id;
    /** Socket used by the server to talk to us (owned by this) */
    sakusen::comms::Socket* incomingSocket;
    /** Socket the server has reserved for us to talk to it (owned by this,
     * but might be the same as incomingSocket, so don't delete both before
     * checking that) */
    sakusen::comms::Socket* outgoingSocket;

    String universeName;

    void initialSettingsSetup();
    void settingAlteration(const String& setting, const String& value);
  public:
    /** \name Accessors */
    //@{
    inline bool isJoined() const { return joined; }
    inline sakusen::comms::ClientID getID() const { return id; }
    //@}
    
    bool getAdvertisement(sakusen::comms::AdvertiseMessageData* advertisement);
    String flushIncoming();
    String join();
    bool leave(bool sendMessage);
    bool send(const sakusen::comms::MessageData& message);
    bool getSetting(const String& setting);
    bool setSetting(const String& setting, const String& value);
    bool setClientSetting(const String& setting, const String& value);
};

}

#endif // SERVERINTERFACE_H

