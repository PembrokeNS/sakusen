#ifndef DISABLE_AVAHI
#ifndef _MDNS_H
#define _MDNS_H

#include "libsakusen-global.h"
#include "servedgame.h"
#include <boost/utility.hpp> // for noncopyable
#include <boost/shared_ptr.hpp> // for shared_ptr
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
 * do any mDNS on our own. However, it seems a bit of a subtle dependency, the
 * edge cases are things that don't seem likely to come up, and it is likely
 * that if the user doesn't have Avahi he isn't using mDNS anywhere else, so
 * Sakusen will (eventually) fall back to using libavahi-core to talk mDNS
 * directly if the Avahi daemon is not found.
 *
 * \todo Fall back to libavahi-core if there is no avahid.
 */

struct AvahiClient;
struct AvahiEntryGroup;
struct AvahiThreadedPoll;

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
 * \todo Pass the AVAHI_PUBLISH_UPDATE flag to avahi_entry_group_add_service()
 * when the portno is updated, and use avahi_entry_group_update_txt() when
 * anything else is changed.
 */
class MdnsPublisher : public boost::noncopyable {
  public:
    MdnsPublisher(boost::shared_ptr<ServedGame const>);
    ~MdnsPublisher();
    void game_changed();
    void create_services(AvahiClient *c);

    /** \brief Access the internal ServedGame.
     *
     * This is for convenience, so users of this class can throw away their
     * pointer to the ServedGame once they have used it to init this class, and
     * then get it back later to update it. Don't forget to call game_changed()
     * after updating it.
     */
    boost::shared_ptr<ServedGame> const getGame() { return boost::const_pointer_cast<ServedGame,ServedGame const>(game); }
  private:
    boost::shared_ptr<ServedGame const> const game;
    /** We need this to pass to avahi.
     *
     * It needs to be a member rather than local to the function it is used
     * because we can't free it until Avahi is shut down in the dtor.
     */
    char *game_name;
    AvahiClient *client;
    AvahiEntryGroup *group;
    AvahiThreadedPoll *poll;
};

}}

#endif //_MDNS_H
#endif //DISABLE_AVAHI
