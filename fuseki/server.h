#ifndef SERVER_H
#define SERVER_H

#include <iosfwd>

#include "gnu_extensions.h"
#include "player.h"
#include "map.h"
#include "resourceinterface.h"
#include "socket.h"
#include "remoteclient.h"
#include "settingstree/settingstree.h"
#include "settingstree/leaf.h"

namespace fuseki {

class Server : public SettingsUser {
  private:
    Server();
    Server(const Server&);
  public:
    Server(
        sakusen::comms::Socket* solicitationSocket,
        sakusen::comms::Socket* joinSocket,
        std::ostream& output,
        sakusen::ResourceInterface* resourceInterface,
        bool abstract,
        bool dots
      ); /* Caller must ensure that socket remains open until the server is
          * out of use */
    ~Server();
  private:
    bool abstract; /* Whether to use abstract sockets for RemoteClients to
                      listen */
    bool dots; /* Whether to print dots constantly */
    sakusen::comms::Socket* solicitationSocket; /* Not owned by this */
    sakusen::comms::Socket* joinSocket; /* Not owned by this */
    std::ostream& out;
    sakusen::ResourceInterface* resourceInterface; /* Not owned by this */
    __gnu_cxx::hash_map<sakusen::comms::ClientID, RemoteClient*> clients;
      /* Client interfaces (owned by this) */
    sakusen::Universe* universe;
      /* The universe we plan to use to build the map.  NULL
                           if none selected yet.  Owned by this. */
    sakusen::MapTemplate* map;
      /* The map template which we plan to use to build the world.
         NULL if none selected yet.  Owned by this. */
    uint32 mapPlayMode; /* Which mode we plan to play the map on */
    std::vector<sakusen::server::Player> players;
      /* Players.  Note that this vector *becomes obsolete* as soon as the game
       * is started, because World makes a copy of it and works with that */
    settingsTree::SettingsTree settings; /* The tree of all the settings */
    bool allowObservers; /* Whether we allow observer clients */
    
    bool checkForGameStartNextTime; /* Indicate that a check for whether the
                                       game can start is in order */
    bool ensureAdminExistsNextTime; /* Indicate that a check for whether an
                                       admin exists is in order */
    sakusen::Universe* requestedUniverse;
      /* Put a universe here to be promoted to universe when possible (owned
       * by this) */
    sakusen::MapTemplate* requestedMap;
      /* Put a map here to be promoted to map when possible (owned by this) */
    bool mapPlayModeChanged;

    uint32 gameSpeed; /* Desired game speed in microseconds per tick */

    sakusen::comms::ClientID getFreeClientID();
      /* Find an unused ClientID for a new client.
       * Returns (ClientID)-1 if there are no free IDs */
    void addClient(
        const String& requestedAddress,
        const String& fromAddress,
        sakusen::comms::Socket* existingSocket
      );
    void removeClient(RemoteClient* client);
    void handleClientMessages();
    void clearPlayers();
    void createPlayersFor(const sakusen::MapPlayMode& mode);
    void setAllowObservers(bool value);
    inline void setMapPlayMode(uint32 mode) {
      mapPlayMode = mode;
      mapPlayModeChanged = true;
    }
    void changeInClientBranch(
        RemoteClient* client,
        const String& node,
        const String& value
      );
  public:
    sakusen::server::Player* getPlayerPtr(sakusen::PlayerID id);
    inline bool getAllowObservers() { return allowObservers; }
    
    void serve(); /* Do what servers do best */
    void checkForGameStart();
      /* Called when a client has reason to believe that it might now be
       * possible for the game to start (e.g. because they just set their
       * ready flag). */
    void ensureAdminExists();
      /* Called to induce the server to create an admin if none already exists.
       * Called if any client loses admin status or leaves or permits admin
       * status */

    /* Functions that are called to advise server that the setting on the given
     * leaf wants to be altered.  Returns reason why it can't be, or null
     * string if it can. */
    String boolSettingAlteringCallback(
        settingsTree::Leaf* altering,
        bool newValue
      );
    template<typename T>
    String intSettingAlteringCallback(
        settingsTree::Leaf* altering,
        T newValue
      );
    String stringSettingAlteringCallback(
        settingsTree::Leaf* altering,
        const String& newValue
      );
    String stringSetSettingAlteringCallback(
        settingsTree::Leaf* altering,
        const __gnu_cxx::hash_set<String, sakusen::StringHash>& newValue
      );

    void settingAlteredCallback(settingsTree::Leaf* altered);
};

}

#endif // SERVER_H

