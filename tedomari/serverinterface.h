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
    ServerInterface(
        sakusen::comms::Socket* serverSocket,
        const String& joinAddress,
        bool unixSockets,
        bool abstract,
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
    bool unixSockets; /* OK to use Unix sockets */
    bool abstract; /* OK to use abstract namespace */
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

    /** Do initial setup of settings for us on the server (e.g. tell our
     * client application, etc.) */
    void initialSettingsSetup();
    void settingAlteration(const String& setting, const String& value);
  public:
    inline bool isJoined() const { return joined; }
    inline sakusen::comms::ClientID getID() const { return id; }
    
    /** Try to get advertisement from server.  Returns true iff an error
     * occurs */
    bool getAdvertisement(sakusen::comms::AdvertiseMessageData* advertisement);
    /** Deal with all pending messages from server.  Returns true iff some
     * output happened */
    String flushIncoming();
    /** \brief Try to join server.
     *
     * \return true iff an error occurs */
    String join();
    /** \brief Try to leave server.
     *
     * \param sendMessage Whether to send a message to the server indicating
     * that we are leaving.
     * \return true iff an error occurs */
    bool leave(bool sendMessage);
    /** \brief Send the given message to the server
     *
     * \param message Message to send
     * \return true iff an error occurs */
    bool send(const sakusen::comms::MessageData& message);
    /* \brief Request a setting from the server (asynchronously).
     *
     * \param setting Address of the setting to request
     * \return true iff an error occurs */
    bool getSetting(const String& setting);
    /* \brief Requests a setting change on the server (asynchronously).
     *
     * \param setting Address of the setting to change
     * \param value Value to assign to the setting
     * \return true iff an error occurs */
    bool setSetting(const String& setting, const String& value);
    /* \brief Requests a setting change  our client settings tree.
     * 
     * \param setting Address of the setting to change relative to the client
     * branch
     * \param value Value to assign to the setting
     * \return true iff an error occurs */
    bool setClientSetting(const String& setting, const String& value);
};

}

#endif // SERVERINTERFACE_H

