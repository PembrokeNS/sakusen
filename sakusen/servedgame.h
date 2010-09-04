#ifndef LIBSAKUSEN__SERVEDGAME_H
#define LIBSAKUSEN__SERVEDGAME_H

#include <sakusen/exceptions.h>

namespace sakusen {

/** \brief Indicates a game name is too long.
 *
 * Game names can be no longer than 63 UTF-8 octets plus a terminating nul.
 */
class GameNameTooLong : public Exn {
  public:
    /** \todo Don't inline. */
    GameNameTooLong() : Exn("Game name too long") { }
};

/** \brief A game being served on the network.
 *
 * Used to transfer details of running or waiting games to and from game
 * browsers. An instance of this class represents a game being served. Game
 * servers will generate an instance of this and pass it to code that registers
 * games, e.g. MdnsPublisher. Clients will be given instances of this by code
 * that finds games, e.g. MdnsBrowser.
 *
 * \bug Store the address of the server, tard.
 *
 * \todo Store the network protocol version, but for a sensible default we need
 * an include from libsak-comms.
 *
 * \todo Store the server application name (and version?).
 */
class ServedGame {
  public:
    /** \brief Standard ctor, initing all fields with sensible defaults.
     * \throws GameNameTooLong in the obvious case.
     */
    ServedGame(String const & game_name,
               int game_port = 1776,
               String const & game_universe = String(),
               String const & game_map = String())
        : name(game_name), port(game_port), universe(game_universe), map(game_map)
    {
      if (name.length() > 63)
        throw GameNameTooLong();
    }

    /** \brief Copy ctor.
     * \throws std::bad_alloc if the string copy fails.
     */
    ServedGame(ServedGame const & original)
      : name(original.name), port(original.port) { }
    /** \brief Almost copy ctor, giving the new ServedGame a new name.
     * \throws GameNameTooLong in the obvious case.
     */
    ServedGame(ServedGame const & original, String const & new_name)
        : name(new_name), port(original.port)
    {
      if (name.length() > 63)
        throw GameNameTooLong();
    }
    /** \name Accessors
     * @{
     */
    String const & getName() const { return name; }

    int getPort() const { return port; }

    String const & getUniverse() const { return universe; }
    /** \param new_universe May be empty. */
    void setUniverse(String const & new_universe) { universe = new_universe; }

    String const & getMap() const { return map; }
    /** \param new_map May be empty. */
    void setMap(String const & new_map) { map = new_map; }
    /**@}*/
  private:
    /** The human-readable game name, e.g. "The Horizon Problem clanserver".
     *
     * The name is a key, in the sense that no two games in the same browsing
     * domain can have the same name. Thus you can't change it without creating
     * a new ServedGame instance. Also, it must be no longer than 63
     * bytes (plus a terminating nul), each of which is a UTF-8 octet.
     */
    const String name;
    /** The address to connect to. */
    /** The port to connect to. */
    int port;
    /** The universe the game is being played in. */
    String universe;
    /** The map the game is being played on. */
    String map;
};

}

#endif /*LIBSAKUSEN__SERVEDGAME_H*/
