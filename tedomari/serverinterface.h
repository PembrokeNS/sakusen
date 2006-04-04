#ifndef SERVERINTERFACE_H
#define SERVERINTERFACE_H

#include <time.h>

#ifndef _MSC_VER
  #include <sys/time.h>
#endif

#include "socket.h"
#include "game/game.h"

namespace tedomari {

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
    sakusen::comms::ClientID id;
      /* ID assigned to us by the server (valid only when joined) */
    sakusen::comms::Socket* incomingSocket;
      /* Socket used by the server to talk to us (owned by this) */
    sakusen::comms::Socket* outgoingSocket;
      /* Socket the server has reserved for us to talk to it (owned by this,
       * but might be the same as incomingSocket, so don't delete both before
       * checking that) */

    String universeName;

    void initialSettingsSetup();
      /* Do initial setup of settings for us on the server (e.g. tell our
       * client application, etc.) */
    void settingAlteration(const String& setting, const String& value);
  public:
    inline bool isJoined() const { return joined; }
    inline sakusen::comms::ClientID getID() const { return id; }
    
    bool getAdvertisement(sakusen::comms::AdvertiseMessageData* advertisement);
      /* Try to get advertisement from server.  Returns true iff an error
       * occurs */
    String flushIncoming();
      /* Deal with all pending messages from server.  Returns true iff some
       * output happened */
    String join();
      /* Try to join server.  Returns true iff an error occurs */
    bool leave(bool sendMessage);
      /* Try to leave server.  Returns true iff an error occurs */
    bool send(const sakusen::comms::MessageData& message);
    bool getSetting(const String& setting);
      /* Get a setting from the server.  Returns true iff an error occurs */
    bool setSetting(const String& setting, const String& value);
      /* Set a setting on the server.  Returns true iff an error occurs */
    bool setClientSetting(const String& setting, const String& value);
      /* Set a setting inside our client settings tree.
       * Returns true iff an error occurs */
};

}

#endif // SERVERINTERFACE_H

