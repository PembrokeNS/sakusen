#ifndef SERVER_H
#define SERVER_H

#include <iosfwd>

#include "gnu_extensions.h"
#include "player.h"
#include "map.h"
#include "resourceinterface.h"
#include "socket.h"
#include "remoteclient.h"
#include "plugin.h"
#include "plugininterface.h"
#include "settingstree/settingstree.h"
#include "settingstree/leaf.h"

namespace fuseki {

/** \brief Represents a server.
 *
 * This class manages all communication with clients, adding and removing
 * clients, maintainance of the settings tree and game initialization and
 * shutdown. */
class Server : public SettingsUser, private boost::noncopyable {
  public:
    /** \brief Standard constructor
     *
     * \param output Stream to use for output.
     * \param resourceInterface ResourceInterface to use to load game data.
     * \param pluginPaths List of paths to be searched (in that order) for
     * plugins when such are requested.
     * \param abstract Whether to use abstract unix socket namespace where
     * possible.
     * \param unixSocket A Socket on which to listen for both
     * solicitation and join messages.
     * \param tcpSocket A Socket on which to listen for join messages.
     * \param dots Whether to print dots so as to indicate that the server is
     * alive.
     *
     * Caller must ensure that the various sockets remain open until the
     * server is out of use.  Ownership of the socket objects is not
     * transferred to the Server. */
    Server(
        std::ostream& output,
        const sakusen::ResourceInterface::Ptr& resourceInterface,
        const std::list<String>& pluginPaths,
#ifndef DISABLE_UNIX_SOCKETS
        bool abstract,
        const sakusen::comms::Socket::Ptr& unixSocket,
#endif
        const sakusen::comms::Socket::Ptr& tcpSocket,
        bool dots
      );
    /** \brief Destructor */
    ~Server();
  private:
    /** Whether to print dots constantly */
    bool dots;
#ifndef DISABLE_UNIX_SOCKETS
    /** Whether to use abstract sockets for RemoteClients to listen */
    bool abstract;
    /** The Unix socket to listen on. */
    sakusen::comms::Socket::Ptr unixSocket;
#endif
    sakusen::comms::Socket::Ptr tcpSocket;
    std::ostream& out;
    sakusen::ResourceInterface::Ptr resourceInterface;
    PluginInterface pluginInterface;
    /** Client interfaces (owned by this) */
    __gnu_cxx::hash_map<sakusen::ClientID, RemoteClient*> clients;
    /** The universe we plan to use to build the map.  NULL
     * if none selected yet.
     */
    sakusen::Universe::ConstPtr universe;
    /* The map template which we plan to use to build the world.
     * NULL if none selected yet. */
    sakusen::MapTemplate::ConstPtr map;
    /** The mode we plan to play the map on.
     * \todo Make this more meaningful than a plain int.
     */
    uint32 mapPlayMode;

    /** Players.  Note that this vector *becomes obsolete* as soon as the game
     * is started, because World makes a copy of it and works with that */
    std::vector<sakusen::server::Player> players;
    /** The settings for this game. */
    settingsTree::SettingsTree::Ptr settings;
    /** True iff we allow clients to connect as observers. */
    bool allowObservers;

    sakusen::hash_map_string<Plugin::Ptr>::type plugins;
    __gnu_cxx::hash_map<
        sakusen::MaskedPtr<sakusen::server::plugins::Listener>,
        sakusen::server::plugins::Listener::VPtr
      > listeners;
    
    bool checkForGameStartNextTime; /* Indicate that a check for whether the
                                       game can start is in order */
    bool ensureAdminExistsNextTime; /* Indicate that a check for whether an
                                       admin exists is in order */
    sakusen::Universe::ConstPtr requestedUniverse;
      /* Put a universe here to be promoted to universe when possible */
    sakusen::MapTemplate::ConstPtr requestedMap;
      /* Put a map here to be promoted to map when possible */
    std::queue<String> pluginsToAdd;
    sakusen::hash_set_string pluginsToRemove;
    bool mapPlayModeChanged;

    bool gameStarted;
    uint32 gameSpeed; /* Desired game speed in microseconds per tick */

    sakusen::ClientID getFreeClientID();
      /* Find an unused ClientID for a new client.
       * Returns (ClientID)-1 if there are no free IDs */
    void addClient(
        const String& requestedAddress,
        const String& fromAddress,
        const sakusen::comms::Socket::Ptr& existingSocket
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
    /** \brief Checks currently loaded plugins against the given list,
     * and sets up addition and removal as appropriate */
    void checkPlugins(const std::set<String>& newList);
  public:
    /** \brief Get the Server's ResourceInterface */
    sakusen::ResourceInterface::Ptr getResourceInterface() const {
      return resourceInterface;
    }
    sakusen::server::Player* getPlayerPtr(sakusen::PlayerID id);
    /** \brief Get the root of the Server's settings tree */
    const settingsTree::SettingsTree::Ptr& getSettings() { return settings; }
    /** \return true iff the server is currently allowing observers */
    inline bool getAllowObservers() const { return allowObservers; }
    /** \brief Determine if the game hosted by the Server has yet started */
    inline bool getGameStarted() const { return gameStarted; }
    
    void serve();
    void checkForGameStart();
    void ensureAdminExists();

    void registerListener(const sakusen::server::plugins::Listener::VPtr&);
    void unregisterListener(
        const sakusen::MaskedPtr<sakusen::server::plugins::Listener>&
      );

    /** \name Settings tree callbacks
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
        const std::set<String>& newValue
      );
    //@}

    void settingAlteredCallback(settingsTree::Leaf* altered);
};

}

#endif // SERVER_H

