#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/thread-watch.h>
#include "libsakusen-global.h"
/** \file
 *
 * Negotiate with Avahi to advertise games via mDNS.
 *
 * For LAN games, clients need a way of finding the server. mDNS (multicast
 * DNS) is a service discovery or rendezvous protocol. Avahi provides both a
 * library for talking mDNS directly, and a daemon that handles all the details
 * of the protocol on servers' and clients' behalf.
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

namespace fuseki {
/** \brief RAII interface to Avahi.
 *
 * Construct an instance to start advertising the service. This will also start
 * a thread for Avahi to listen on. Destruct the instance to stop advertising
 * the service. This class is not thread-safe, in that each instance must be
 * accessed from only a single thread, but you can have as many instances as
 * you like, each accessed from a possibly different thread. Each will get its
 * own Avahi event loop thread.
 */
class MdnsPublisher {
  public:
    MdnsPublisher(const std::string name="sakusen", uint32 portno=1776);
    ~MdnsPublisher();
    /** \brief Internal function. Do not call from outside this file.
     * 
     * This function has to be public because internal non-methods need to call
     * it, but you should not. It creates the service for Avahi to advertise,
     * from the information you have already given the ctor.
     */
    void create_services(AvahiClient *c);
  private:
    uint32 port;
    /** The name of the server, as supplied to Avahi.
     *
     * Needs to be a cstr, I'm afraid.
     */
    char * game_name;
    AvahiClient *client;
    AvahiThreadedPoll *poll;
};

}
