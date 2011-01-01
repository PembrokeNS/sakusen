#ifndef FUSEKI__SERVER_H
#define FUSEKI__SERVER_H

#include "global.h"

#include <iosfwd>

#include <boost/ptr_container/ptr_list.hpp>

#include <sakusen/gnu_extensions.h>
#include <sakusen/server/player.h>
#include <sakusen/map.h>
#include <sakusen/resourceinterface.h>
#include <sakusen/comms/socket.h>
#include "remoteclient.h"
#include "plugin.h"
#include "plugininterface.h"
#include <sakusen/servedgame.h>
#include "settingstree/settingstree.h"
#include "settingstree/leaf.h"

namespace fuseki {

class ServerAction;

/** \brief Represents a server.
 *
 * This class manages all communication with clients, adding and removing
 * clients, maintainance of the settings tree and game initialization and
 * shutdown. */
class Server : public SettingsUser, private boost::noncopyable {
  friend class ClientChangeSettingAction;
  friend class EnsureAdminExistsAction;
  friend class RequestUniverseAction;
  friend class RequestMapAction;
  friend class SetMapPlayModeAction;
  friend class AddPluginAction;
  friend class RemovePluginAction;
  friend class CheckForGameStartAction;
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
        const std::vector<boost::filesystem::path>& pluginPaths,
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
    u_map<sakusen::ClientId, RemoteClient*>::type clients;
    /** The universe we plan to use to build the map.  NULL
     * if none selected yet.
     */
    sakusen::Universe::ConstPtr universe;
    String universePath;
    /* The map template which we plan to use to build the world.
     * NULL if none selected yet. */
    sakusen::MapTemplate::ConstPtr map;
    String mapPath;
    /** The mode we plan to play the map on.
     * \todo Make this more meaningful than a plain int.
     */
    uint32 mapPlayMode;

    boost::shared_ptr<sakusen::ServedGame> gameToAdvertise;
    bool gameToAdvertiseChanged;
    bool startGame;

    /** Players.  Note that this vector *becomes obsolete* as soon as the game
     * is started, because World makes a copy of it and works with that */
    std::vector<sakusen::server::Player> players;
    /** The settings for this game. */
    settingsTree::SettingsTree::Ptr settings;
    /** True iff we allow clients to connect as observers. */
    bool allowObservers;

    u_map<String, Plugin::Ptr>::type plugins;
    u_map<
        sakusen::MaskedPtr<sakusen::server::plugins::Listener>,
        sakusen::server::plugins::Listener::VPtr
      >::type listeners;

    boost::ptr_list<ServerAction> pendingActions;

    bool gameStarted;
    uint32 gameSpeed; /* Desired game speed in microseconds per tick */

    /** \brief Find an unused ClientId for a new client.
     *
     * \returns A free ClientId, or (ClientId)-1 if there are no free ids. */
    sakusen::ClientId getFreeClientId();
    void addClient(
        const String& requestedAddress,
        const String& fromAddress,
        const sakusen::comms::Socket::Ptr& existingSocket
      );
    void removeClient(RemoteClient* client);
    void handleClientMessages();
    void handlePendingActions();
    void clearPlayers();
    void createPlayersFor(const sakusen::MapPlayMode& mode);
    void setAllowObservers(bool value);
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
    sakusen::server::Player* getPlayerPtr(sakusen::PlayerId id);
    /** \brief Get the root of the Server's settings tree */
    const settingsTree::SettingsTree::Ptr& getSettings() { return settings; }
    /** \return true iff the server is currently allowing observers */
    inline bool getAllowObservers() const { return allowObservers; }
    /** \brief Determine if the game hosted by the Server has yet started */
    inline bool isGameStarted() const { return gameStarted; }

    void serve();
    void checkForGameStart();
    void ensureAdminExists();

    void registerListener(const sakusen::server::plugins::Listener::VPtr&);
    void unregisterListener(
        const sakusen::MaskedPtr<sakusen::server::plugins::Listener>&
      );

    template<typename T>
    String settingAlteringCallback(
        settingsTree::Leaf* altering,
        T newValue
      );

    void settingAlteredCallback(settingsTree::Node::Ptr altered);
};

}

#endif // FUSEKI__SERVER_H

