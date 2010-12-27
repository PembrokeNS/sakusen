#ifndef DISABLE_AVAHI

#include <sakusen/server/mdns.h>
#include <iostream>

#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/thread-watch.h>
#include <avahi-common/defs.h>
#include <avahi-common/alternative.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/timeval.h>
#include <avahi-common/strlst.h>

/** The service name we will advertise over mDNS. */
#define MDNS_SERVICE_TYPE "_sakusen._tcp"
/** The TXT record version number, as recommended by RFC. */
#define TXTVERS "1"

/* Writing output from the avahi thread causes false-positives in thread error
 * checking tools; to easily turn off such output we define a local debugging
 * macro */
#ifdef SAUKUSEN_DEBUG_AVAHI
  #define SAKUSEN_SERVER_MDNS_DEBUG(x) SAKUSEN_DEBUG(x)
#else
  #define SAKUSEN_SERVER_MDNS_DEBUG(x)
#endif

namespace sakusen {
namespace server {

class MdnsPublisher::Implementation : public boost::noncopyable {
  public:
    Implementation(boost::shared_ptr<ServedGame const>);
    ~Implementation();
    void game_changed();

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
    static void entry_group_callback(
        AvahiEntryGroup *g,
        AvahiEntryGroupState state,
        void *userdata);
    static void client_callback(
        AvahiClient *c,
        AvahiClientState state,
        void * userdata);
    void create_services(AvahiClient *c);
};

/** \brief Callback for libavahi-client.
 *
 * Passed to avahi_entry_group_new, this cb handles keeping the mDNS entry
 * valid and printing diagnostics. It will be called whenever the entry group
 * state changes.
 *
 * \bug This is slightly thread-unsafe, in that the this ptr might have become
 * invalid since the cb was registered, but as the dtor immediately stops the
 * Avahi thread it is unlikely to cause a problem.
 *
 * The prototype is mandated by avahi.
 */
void MdnsPublisher::Implementation::entry_group_callback(
    AVAHI_GCC_UNUSED AvahiEntryGroup *g,
    AvahiEntryGroupState state,
    AVAHI_GCC_UNUSED void *userdata) {

  switch(state) {
    case AVAHI_ENTRY_GROUP_ESTABLISHED:
      std::cerr << "Service successfully advertised on mDNS." << std::endl;
      break;

    case AVAHI_ENTRY_GROUP_COLLISION:
      /* Theoretically we should pick a new name at this point. */
      std::cerr << "Service name collision." << std::endl;
      break;

    case AVAHI_ENTRY_GROUP_FAILURE:
      std::cerr << "Entry group failure." << std::endl;
      break;

    case AVAHI_ENTRY_GROUP_UNCOMMITED:
      SAKUSEN_SERVER_MDNS_DEBUG("Avahi advert enters uncommitted state.");
      break;

    case AVAHI_ENTRY_GROUP_REGISTERING:
      SAKUSEN_SERVER_MDNS_DEBUG("Avahi advert enters registering state.");
      break;

    default:
      SAKUSEN_SERVER_MDNS_DEBUG("Avahi advert enters unknown state.");
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
void MdnsPublisher::Implementation::client_callback(
    AvahiClient *c,
    AvahiClientState state,
    void * userdata) {
  Implementation *self = reinterpret_cast<Implementation*>(userdata);
  switch (state) {
    case AVAHI_CLIENT_S_RUNNING:
      SAKUSEN_DEBUG("Avahi enters running state");
      self->create_services(c);
      break;

    case AVAHI_CLIENT_S_COLLISION:
      SAKUSEN_DEBUG("Avahi enters collision state");
      break;

    case AVAHI_CLIENT_FAILURE:
      std::cerr << "Avahi client failure: " << avahi_strerror(avahi_client_errno(c)) << std::endl;
      break;

    case AVAHI_CLIENT_CONNECTING:
      SAKUSEN_DEBUG("Avahi enters connecting state");
      break;

    case AVAHI_CLIENT_S_REGISTERING:
      SAKUSEN_DEBUG("Avahi enters registering state");
      break;

    default:
      SAKUSEN_DEBUG("Avahi enters unknown state.");
      break;
  }
}

/** \brief Advertise a game on mDNS.
 *
 * Advertises the given ServedGame on mDNS.
 *
 * \param game_to_advertise Must not be empty.
 */
MdnsPublisher::MdnsPublisher(boost::shared_ptr<ServedGame const> game_to_advertise)
    : pimpl(new Implementation(game_to_advertise))
{ }

/** \brief Advertise a game on mDNS.
 *
 * Does the real work.
 */
MdnsPublisher::Implementation::Implementation(boost::shared_ptr<ServedGame const> game_to_advertise)
    : game(game_to_advertise), game_name(0), client(NULL), group(NULL), poll(NULL) {
  int error = 0;

  assert(game.get()); // check for emptiness
  game_name = avahi_strdup(game->getName().c_str());
  poll = avahi_threaded_poll_new();
  if (!poll) {
    std::cerr << "Failed to create threaded poll object." << std::endl;
    goto fail;
  }

  client = avahi_client_new(avahi_threaded_poll_get(poll), static_cast<AvahiClientFlags>(0), client_callback, this, &error);
  if (!client) {
    std::cerr << "Failed to create Avahi client." << std::endl;
    goto fail;
  }

  /* Start the thread (this function has a return value, but I don't know what
   * it means) */
  avahi_threaded_poll_start(poll);

  return;
fail:
  if (client) {
    avahi_client_free(client);
    client = NULL;
    group = NULL;
  }

  if (poll) {
    avahi_threaded_poll_free(poll);
    poll = NULL;
  }
}

/** \brief Turn the data from game into an AvahiStringList suitable for making TXT records.
 *
 * \returns A pointer to an allocated AvahiStringList. You must free it when
 * you are done by passing it to avahi_string_list_free().
 */
static AvahiStringList * game_to_txt_records(boost::shared_ptr<ServedGame const> game) {
  /* start with an empty list */
  AvahiStringList *result =
    avahi_string_list_new("txtvers=" TXTVERS, static_cast<const void*>(NULL));
  if (!result)
    throw std::bad_alloc();

  if (!game->getUniverse().empty())
    result = avahi_string_list_add_pair(result, "universe", game->getUniverse().c_str());
  if (!game->getMap().empty())
    result = avahi_string_list_add_pair(result, "map", game->getMap().c_str());

  return result;
}

/** \brief Update mDNS info to reflect changes to \c game.
 *
 * If you change the ServedGame you passed the ctor a pointer to, you should
 * call this method shortly afterwards to push the update out on the ether.
 */
void MdnsPublisher::game_changed() {
  pimpl->game_changed();
}

/** \brief Update mDNS info to reflect changes to \c game.
 *
 * Does all the real work.
 */
void MdnsPublisher::Implementation::game_changed() {
  if (group) {
    AvahiStringList *txt_records = game_to_txt_records(game);

    avahi_threaded_poll_lock(poll);

    if (avahi_client_get_state(client) != AVAHI_CLIENT_S_RUNNING) {
      SAKUSEN_DEBUG("Avahi client in unexpected state on update");
    }

    if (avahi_entry_group_get_state(group) != AVAHI_ENTRY_GROUP_ESTABLISHED) {
      SAKUSEN_DEBUG("Avahi advert in unexpected state on update.");
    }
    
    avahi_entry_group_update_service_txt_strlst(group,
        AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
        static_cast<AvahiPublishFlags>(0),
        game_name,
        MDNS_SERVICE_TYPE, NULL,
        txt_records);

    avahi_string_list_free(txt_records);
    avahi_threaded_poll_unlock(poll);
  }
}

/** \brief Create and commit the services to be advertised.
 */
void MdnsPublisher::Implementation::create_services(AvahiClient *c) {

  group = avahi_entry_group_new(c, entry_group_callback, this);
  if (!group) {
    std::cerr << "Failed to create Avahi advert: " << avahi_strerror(avahi_client_errno(client)) << std::endl;
  } else {
    AvahiStringList *txt_records = game_to_txt_records(game);
    int ret;

    ret = avahi_entry_group_add_service_strlst(group,
      AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
      static_cast<AvahiPublishFlags>(0),
      game_name,
      MDNS_SERVICE_TYPE, NULL, NULL,
      game->getPort(), txt_records);

    if (ret < 0) {
      std::cerr << "Failed to add service: " << avahi_strerror(ret) << std::endl;
    } else {

      ret = avahi_entry_group_commit(group);

      if (ret < 0) {
        std::cerr << "Failed to publish Avahi advert: " << avahi_strerror(ret) << std::endl;
      }
    }
    avahi_string_list_free(txt_records);
  }
}


/** \brief Shut off the mDNS.
 *
 * Stops the Avahi client and frees all internal memory.
 */
MdnsPublisher::~MdnsPublisher() {
}

/** \brief Shut off the mDNS.
 *
 * Does all the real work.
 */
MdnsPublisher::Implementation::~Implementation() {
  if (poll) {
    avahi_threaded_poll_stop(poll);
  }

  if (game_name) {
    avahi_free(game_name);
    game_name = NULL;
  }
  if (client) {
    avahi_client_free(client);
    client = NULL;
    group = NULL;
  }
  if (poll) {
    avahi_threaded_poll_free(poll);
    poll = NULL;
  }
  if (group) {
    avahi_entry_group_free(group);
  }
}

}}

#endif //DISABLE_AVAHI

