#ifndef LIBSAKUSEN_SERVER_PLUGINS__LISTENER_H
#define LIBSAKUSEN_SERVER_PLUGINS__LISTENER_H

#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

#include "client.h"
#include "message.h"

namespace sakusen {
namespace server {
namespace plugins {

/** \brief Interface for something which listens to some or all of the
 * client/server communication.
 *
 * This class is written with replay recorders in mind.  If and when more
 * applications for it are desired or created, it should probably be thought
 * through more carefully.
 */
class Listener {
  public:
    typedef boost::shared_ptr<Listener> Ptr;
    typedef boost::shared_ptr<const Listener> ConstPtr;
    typedef boost::variant<Listener*, Ptr> VPtr;

    virtual ~Listener() {}
    /** \brief Callback at game start
     *
     * This function is called on each Listener at game start, immediately
     * after sakusen::server::world is assigned to.  The initial state of the
     * game can be determined by examining *world. */
    virtual void gameStart() {}

    /** \brief Callback at each message from a client
     *
     * This function will be called on each listener every time a message is
     * read by the server from a client. */
    virtual void clientMessage(const Client*, const comms::Message&) {}

    /** \brief Callback just before each tick's gamestate change
     *
     * This function will be called on each listener once each tick, after all
     * incoming client messages have been processed, but before the state of
     * the world is changed. */
    virtual void ticksMessagesDone() {}
    
    /** \brief Callback after each tick's gamestate change
     *
     * This function will be called on each listener once each tick, after the
     * state of the world is changed, but before any messages are read for the
     * next tick. */
    virtual void tickDone() {}
};

}}}

#endif // LIBSAKUSEN_SERVER_PLUGINS__LISTENER_H

