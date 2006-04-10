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

/** \brief Represents a server.
 *
 * This class manages all communication with clients, adding and removing
 * clients, maintainance of the settings tree and game initialization and
 * shutdown. */
class Server : public SettingsUser {
  private:
    Server();
    Server(const Server&);
  public:
    /** \brief Standard constructor
     *
     * \param output Stream to use for output.
     * \param resourceInterface ResourceInterface to use to load game data.
     * \param abstract Whether to use abstract unix socket namespace where
     * possible.
     * \param unixSocket A Socket on which to listen for both
     * solicitation and join messages.
     * \param udpSocket A Socket on which to listen for solicitation messages.
     * \param tcpSocket A Socket on which to listen for join messages.
     * \param dots Whether to print dots so as to indicate that the server is
     * alive.
     *
     * Caller must ensure that the various sockets remain open until the
     * server is out of use.  Ownership of the socket objects is not
     * transferred to the Server. */
    Server(
        std::ostream& output,
        sakusen::ResourceInterface* resourceInterface,
#ifndef DISABLE_UNIX_SOCKETS
        bool abstract,
        sakusen::comms::Socket* unixSocket,
#endif
        sakusen::comms::Socket* udpSocket,
        sakusen::comms::Socket* tcpSocket,
        bool dots
      );
    ~Server();
  private:
    /** Whether to print dots constantly */
    bool dots;
#ifndef DISABLE_UNIX_SOCKETS
     /** Whether to use abstract sockets for RemoteClients to listen */
    bool abstract;
    sakusen::comms::Socket* unixSocket;
#endif
    sakusen::comms::Socket* udpSocket; /* Not owned by this */
    sakusen::comms::Socket* tcpSocket; /* Not owned by this */
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

    void advertise(
        const sakusen::comms::SolicitMessageData& data,
        const String& receivedFrom,
        sakusen::comms::Socket* receivedOn,
        const sakusen::comms::Message& advertisement
      );
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
    
    void serve();
    void checkForGameStart();
    void ensureAdminExists();

    /* \name Settings tree callbacks
     *
     * These methods are called by leaves of the settings tree to advise the
     * server that the setting on the given
     * leaf wants to be altered.  These methods do not themselves perform any
     * alteration of the settings tree, but they do execute the associated
     * magic.
     *
     * \param altering The leaf being altered.
     * \param newValue The new value of the leaf.
     * \return Empty string if the alteration may be performed, or a reason why
     * it cannot be performed otherwise. */
    //@{
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
    //@}

    void settingAlteredCallback(settingsTree::Leaf* altered);
};

}

#endif // SERVER_H

