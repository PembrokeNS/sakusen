#include "libsakusen-global.h"
#include "mdnsbrowser.h"
#include <iostream> // std::cerr
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>

#include <avahi-common/thread-watch.h>
#include <avahi-common/defs.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/strlst.h>

/** \file
 * Contains the implementation of MdnsBrowser.
 */

/** The service name we will look for. */
#define MDNS_SERVICE_TYPE "_sakusen._tcp"

/** The version number for TXT fields.
 *
 * Hopefully we will never have to change it.
 */
#define TXTVERS "1"

namespace sakusen {
namespace client {

class MdnsBrowser::Implementation : public boost::noncopyable {
  public:
  Implementation(
      boost::function<void(std::auto_ptr<ServedGame>)> cb_appeared,
      boost::function<void(String)> cb_gone);
  ~Implementation();

  private:
  AvahiClient *client;
  AvahiThreadedPoll *poll;
  AvahiServiceBrowser *sb;
  boost::function<void(std::auto_ptr<ServedGame>)> game_appeared;
  boost::function<void(String)> game_gone;

  static void resolve_callback(
    AvahiServiceResolver *r,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiResolverEvent event,
    const char *name,
    const char *type,
    const char *domain,
    const char *host_name,
    const AvahiAddress *address,
    uint16_t port,
    AvahiStringList *txt,
    AvahiLookupResultFlags flags,
    void* userdata);

  static void browse_callback(
    AvahiServiceBrowser *b,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiBrowserEvent event,
    const char *name,
    const char *type,
    const char *domain,
    AvahiLookupResultFlags flags,
    void* userdata);

  static void client_callback(
    AvahiClient *c,
    AvahiClientState state,
    void * userdata);
};
/** \brief Turn Avahi data into a ServedGame.
 *
 * \param name The name of the server as supplied by Avahi.
 * \param port The port the server is running on, as supplied by Avahi.
 * \param records The TXT records describing the game. Must not be NULL, and
 * will not be modified or freed within this function.
 * \return A pointer to a ServedGame encapsulating the supplied information.
 */
static std::auto_ptr<ServedGame> txt_records_to_game(String name, int port, AvahiStringList const * records) {
  assert(records != NULL);
  std::auto_ptr<ServedGame> result(new ServedGame(name, port));
  /** \bug Function not written. */

  return result;
}

/** \brief Callback for libavahi-client.
 *
 * Passed to avahi_resolve_service, this cb is called when a service has been
 * resolved, and handles calling the client's callback with a new ServedGame.
 *
 * The lengthy prototype is mandated by avahi.
 */
void MdnsBrowser::Implementation::resolve_callback(
    AvahiServiceResolver *r,
    AVAHI_GCC_UNUSED AvahiIfIndex interface,
    AVAHI_GCC_UNUSED AvahiProtocol protocol,
    AvahiResolverEvent event,
    const char *name,
    AVAHI_GCC_UNUSED const char *type,
    AVAHI_GCC_UNUSED const char *domain,
    AVAHI_GCC_UNUSED const char *host_name,
    AVAHI_GCC_UNUSED const AvahiAddress *address,
    uint16_t port,
    AvahiStringList *txt,
    AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
    AVAHI_GCC_UNUSED void* userdata) {
  MdnsBrowser::Implementation *self = reinterpret_cast<MdnsBrowser::Implementation*>(userdata);
  switch (event) {
    case AVAHI_RESOLVER_FAILURE:
      /* The service timed out. How rude. */
      break;

    case AVAHI_RESOLVER_FOUND:
      /* Good news! Good news! */
      if (address->proto != AVAHI_PROTO_INET) {
        std::cerr << "Avahi returned an IPv6 server, no use to us." << std::endl;
        return;
      }
      // do something with address->ipv4.address, which is a uint32 in network byte order
      std::auto_ptr<ServedGame> result = txt_records_to_game(name, port, txt);
      if (self->game_appeared)
        self->game_appeared(result);
      break;
  }

  avahi_service_resolver_free(r);
}

/** \brief Callback for libavahi-client.
 *
 * This cb is called when a service has appeared or disappeared, before we know any of its details.
 *
 * The prototype is mandated by avahi.
 */
void MdnsBrowser::Implementation::browse_callback(
    AvahiServiceBrowser *b,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiBrowserEvent event,
    const char *name,
    const char *type,
    const char *domain,
    AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
    void* userdata) {
  MdnsBrowser::Implementation *self = reinterpret_cast<MdnsBrowser::Implementation*>(userdata);

  switch(event) {
    case AVAHI_BROWSER_FAILURE:
      std::cerr << "Avahi browser failure: " << avahi_strerror(avahi_client_errno(avahi_service_browser_get_client(b)));
      break;

    case AVAHI_BROWSER_NEW:
      if (!avahi_service_resolver_new(self->client,
            interface, protocol,
            name, type, domain,
            AVAHI_PROTO_UNSPEC, static_cast<AvahiLookupFlags>(0),
            resolve_callback, self))
        Debug("Avahi failed to create a service resolver.");
      // That's resolver, not revolver.
      break;

    case AVAHI_BROWSER_REMOVE:
      if (self->game_gone)
        self->game_gone(name);
      break;

    case AVAHI_BROWSER_ALL_FOR_NOW:
      Debug("Avahi said that's all for now.");
      break;

    case AVAHI_BROWSER_CACHE_EXHAUSTED:
      Debug("Avahi said its cache has been exhausted.");
      break;
  }
}


/** \brief Callback for libavahi-client.
 *
 * Passed to avahi_client_new, this cb handles client and server state changes.
 *
 * \bug This is slightly thread-unsafe, in that the this ptr might have become
 * invalid since the cb was registered, but as the dtor immediately stops the
 * Avahi thread it is unlikely to cause a problem.
 *
 * The prototype is mandated by avahi.
 */
void MdnsBrowser::Implementation::client_callback(AvahiClient *c, AvahiClientState state, void * userdata) {
  MdnsBrowser::Implementation *self = reinterpret_cast<MdnsBrowser::Implementation*>(userdata);
  switch (state) {
    case AVAHI_CLIENT_S_RUNNING:
      Debug("Avahi enters running state");
      break;

    case AVAHI_CLIENT_FAILURE:
      std::cerr << "Avahi client failure: " << avahi_strerror(avahi_client_errno(c)) << std::endl;
      avahi_threaded_poll_quit(self->poll);
      break;

    case AVAHI_CLIENT_CONNECTING:
      Debug("Avahi enters connecting state");
      break;

    default:
      Debug("Avahi enters unknown state.");
      break;
  }
}

/** \brief Connect to Avahi and start looking for games.
 *
 * \param cb_appeared This callback will later be called (from a different
 * thread) each time a game appears. It may be empty.
 * \param cb_gone This callback will later be called (from a different thread)
 * when a game disappears. Its parameter is the name of the game. It may be
 * empty.
 */
MdnsBrowser::MdnsBrowser(
    boost::function<void(std::auto_ptr<ServedGame>)> cb_appeared,
    boost::function<void(String)> cb_gone)
      : pimpl(new Implementation(cb_appeared, cb_gone))
{ }

/** \brief Connect to Avahi and start looking for games.
 *
 * This is the real constructor that does all the work.
 */
MdnsBrowser::Implementation::Implementation(
    boost::function<void(std::auto_ptr<ServedGame>)> cb_appeared,
    boost::function<void(String)> cb_gone)
      : client(0), poll(0), sb(0), game_appeared(cb_appeared), game_gone(cb_gone)
{
  int error = 0;

  poll = avahi_threaded_poll_new();
  if (!poll) {
    std::cerr << "Failed to create threaded poll object." << std::endl;
  } else {
    client = avahi_client_new(avahi_threaded_poll_get(poll), static_cast<AvahiClientFlags>(0), client_callback, this, &error);
    if (!client) {
      std::cerr << "Failed to create Avahi client." << std::endl;
      avahi_threaded_poll_free(poll);
    } else {
      avahi_service_browser_new(client,
          AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
          MDNS_SERVICE_TYPE, NULL,
          static_cast<AvahiLookupFlags>(0),
          browse_callback, this);
      avahi_threaded_poll_start(poll);
    }
  }
}

/** \brief Shut off the mDNS.
 *
 * Stops the Avahi client and frees all internal memory.
 */
MdnsBrowser::~MdnsBrowser() {
}

/** \brief Shut off the mDNS.
 *
 * This is the real destructor and does all the work.
 */
MdnsBrowser::Implementation::~Implementation() {
  if (poll)
    avahi_threaded_poll_stop(poll);

  if (sb) {
    avahi_service_browser_free(sb);
    sb = NULL;
  }

  if (client) {
    avahi_client_free(client);
    client = NULL;
  }
  if (poll) {
    avahi_threaded_poll_free(poll);
    poll = NULL;
  }
}

}}
