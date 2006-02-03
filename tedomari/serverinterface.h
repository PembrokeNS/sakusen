#ifndef SERVERINTERFACE_H
#define SERVERINTERFACE_H

#include <time.h>
#include <sys/time.h>

#include <queue>

#include "socket.h"

namespace tedomari {

class ServerInterface {
  private:
    ServerInterface();
    ServerInterface(const ServerInterface& copy);
  public:
    ServerInterface(sakusen::comms::Socket* serverSocket, bool abstract);
    ~ServerInterface();
  private:
    sakusen::comms::Socket* serverSocket;
      /* Socket for solicitation/advertising (owned by code that constructs
       * this object) */
    struct timeval timeout;
    bool abstract; /* OK to use abstract namespace */
    bool joined;
    sakusen::comms::ClientID id;
      /* ID assigned to us by the server (valid only when joined) */
    sakusen::comms::Socket* localSocket;
      /* Socket used by the server to talk to us (owned by this) */
    sakusen::comms::Socket* privateServerSocket;
      /* Socket the server has reserved for us to talk to it (owned by this) */
    bool gameStarted;
    std::queue<sakusen::Update> updateQueue;

    void initialSettingsSetup();
      /* Do initial setup of settings for us on the server (e.g. tell our
       * client application, etc.) */
  public:
    inline bool isJoined() const { return joined; }
    inline sakusen::comms::ClientID getID() const { return id; }
    inline bool isGameStarted() const { return gameStarted; }
    inline bool isUpdateQueueEmpty() const { return updateQueue.empty(); }
    inline sakusen::Update updateQueuePopFront() {
      assert(!updateQueue.empty());
      sakusen::Update update = updateQueue.front();
      updateQueue.pop();
      return update;
    }
    
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

