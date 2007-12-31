#ifndef FUSEKI__SERVERACTION_H
#define FUSEKI__SERVERACTION_H

#include "server.h"

namespace fuseki {

/** \brief Object to abstract the actions that must be performed by the
 * server.
 *
 * To prevent code bloat in Server::serve and help with asyncronous requests
 * and otherwise performing actions in
 * the correct order, various things the server does are abstracted out to
 * classes derived from ServerAction.  These actions are queued up in the
 * server. */
class ServerAction : boost::noncopyable {
  protected:
    ServerAction() {}
  public:
    virtual ~ServerAction() = 0;
    virtual void act(Server&) const = 0;
};

class ClientChangeSettingAction : public ServerAction {
  public:
    ClientChangeSettingAction(
        sakusen::ClientId cI,
        const sakusen::comms::ChangeSettingMessageData& d
      ) :
      clientId(cI), data(d) {}
    void act(Server&) const;
  private:
    sakusen::ClientId clientId;
    sakusen::comms::ChangeSettingMessageData data;
};

/** \brief Check to see whether an admin exists
 *
 * This causes the server to check whether
 * any client has admin status.  If there is no such client, and one is willing
 * to assume admin status, then that client is automatically promoted to admin
 * status.
 * 
 * This is used if any client loses admin status or leaves or permits admin
 * status. */
class EnsureAdminExistsAction : public ServerAction {
  public:
    void act(Server&) const;
};

class RequestUniverseAction : public ServerAction {
  public:
    RequestUniverseAction(const sakusen::Universe::ConstPtr& u, const String& p)
      : universe(u), path(p) {}
    void act(Server&) const;
  private:
    const sakusen::Universe::ConstPtr universe;
    const String path;
};

class RequestMapAction : public ServerAction {
  public:
    RequestMapAction(const sakusen::MapTemplate::ConstPtr& m, const String& p) :
      map(m), path(p) {}
    void act(Server&) const;
  private:
    const sakusen::MapTemplate::ConstPtr map;
    const String path;
};

class SetMapPlayModeAction : public ServerAction {
  public:
    SetMapPlayModeAction(uint32 m) : mode(m) {}
    void act(Server&) const;
  private:
    const uint32 mode;
};

class AddPluginAction : public ServerAction {
  public:
    AddPluginAction(const String& n) : name(n) {}
    void act(Server&) const;
  private:
    const String name;
};

class RemovePluginAction : public ServerAction {
  friend class AddPluginAction; /* Needed for debugging */
  public:
    RemovePluginAction(const String& n) : name(n) {}
    void act(Server&) const;
  private:
    const String name;
};

/** \brief Check for game start
 *
 * This causes the server to check whether the
 * game can be started, and, if it can, then start the game.
 *
 * This is used when something has reason to believe that it might now be
 * possible for the game to start (e.g. a client has just set
 * their ready flag). */
class CheckForGameStartAction : public ServerAction {
  public:
    void act(Server&) const;
};

}

#endif // FUSEKI__SERVERACTION_H

