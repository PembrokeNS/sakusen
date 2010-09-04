#ifndef LIBSAKUSEN_SERVER__MDNS_H
#define LIBSAKUSEN_SERVER__MDNS_H

#include <sakusen/server/libsakusen-server-global.h>
#include <sakusen/servedgame.h>
#include <boost/utility.hpp> // for noncopyable
#include <boost/shared_ptr.hpp> // for shared_ptr
#include <boost/scoped_ptr.hpp> // for scoped_ptr
/** \file
 *
 * Negotiate with Avahi to advertise games via mDNS.
 *
 * Clients need a way of finding the server. mDNS (multicast DNS) is a service
 * discovery or rendezvous protocol. Avahi provides both a library for talking
 * mDNS directly, and a daemon that handles all the details of the protocol on
 * servers' and clients' behalf.
 *
 * There are some features in mDNS that have interesting edge cases that
 * slightly break if you have more than one mDNS endpoint on the same computer,
 * so it is recommended that we talk to the Avahi daemon and don't attempt to
 * do any mDNS on our own.
 */

namespace sakusen {
namespace server {

/** \brief RAII interface to Avahi.
 *
 * Construct an instance to start advertising the service. This will also start
 * a thread for Avahi to listen on. Destruct the instance to stop advertising
 * the service. This class is not thread-safe, in that each instance must be
 * accessed from only a single thread, but you can have as many instances as
 * you like, each accessed from a possibly different thread. Each will get its
 * own Avahi event loop thread.
 *
 * Uses the pimpl idiom to hide Avahi details from callers.
 *
 * \todo Pass the AVAHI_PUBLISH_UPDATE flag to avahi_entry_group_add_service()
 * when the portno is updated.
 *
 * \bug There are some things that appear to be memory leaks in avahi (see
 * tools/valgrind/sakusen.supp).  They may be our fault or avahi's.
 * It would be nice to track them down at some
 * point, but that probably requires understanding the avahi source.
 */
class LIBSAKUSEN_SERVER_API MdnsPublisher : public boost::noncopyable {
  public:
    MdnsPublisher(boost::shared_ptr<ServedGame const>);
    ~MdnsPublisher();
    void game_changed();

    /** \brief Access the internal ServedGame.
     *
     * This is for convenience, so users of this class can throw away their
     * pointer to the ServedGame once they have used it to init this class, and
     * then get it back later to update it. Don't forget to call game_changed()
     * after updating it.
     */
    boost::shared_ptr<ServedGame> const getGame() { return boost::const_pointer_cast<ServedGame,ServedGame const>(game); }
  private:
    /** \note Here rather than in Implementation chiefly so that getGame() can
     * be inlined.
     */
    boost::shared_ptr<ServedGame const> const game;

    class Implementation;
    boost::scoped_ptr<Implementation> pimpl;

};

}}

#endif // LIBSAKUSEN_SERVER__MDNS_H

