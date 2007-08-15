#ifndef DISABLE_AVAHI

#include "mdns.h"
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

/** \file
 *
 * \todo Refactor this code to look less like the C-ish mess that is the
 * example code.
 */

/** The service name we will advertise over mDNS. */
#define MDNS_SERVICE_TYPE "_sakusen._tcp"
/** The TXT record version number, as recommended by RFC. */
#define TXTVERS "1"

namespace sakusen {
namespace server {
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
void entry_group_callback(AVAHI_GCC_UNUSED AvahiEntryGroup *g, AvahiEntryGroupState state, AVAHI_GCC_UNUSED void *userdata) {
  //MdnsPublisher *self = reinterpret_cast<MdnsPublisher*>(userdata);

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
      Debug("Avahi advert enters uncommitted state.");
      break;

    case AVAHI_ENTRY_GROUP_REGISTERING:
      Debug("Avahi advert enters registering state.");
      break;

    default:
      Debug("Avahi advert enters unknown state.");
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
void client_callback(AvahiClient *c, AvahiClientState state, void * userdata) {
  MdnsPublisher *self = reinterpret_cast<MdnsPublisher*>(userdata);
  switch (state) {
    case AVAHI_CLIENT_S_RUNNING:
      Debug("Avahi enters running state");
      self->create_services(c);
      break;

    case AVAHI_CLIENT_S_COLLISION:
      Debug("Avahi enters collision state");
      break;

    case AVAHI_CLIENT_FAILURE:
      std::cerr << "Avahi client failure: " << avahi_strerror(avahi_client_errno(c)) << std::endl;
      break;

    case AVAHI_CLIENT_CONNECTING:
      Debug("Avahi enters connecting state");
      break;

    case AVAHI_CLIENT_S_REGISTERING:
      Debug("Avahi enters registering state");
      break;

    default:
      Debug("Avahi enters unknown state.");
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
    : game(game_to_advertise), client(NULL), group(NULL), poll(NULL) {
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
 *
 * \note This is not a member function to avoid having to include
 * avahi_strlst.h from mdns.h.
 */
static AvahiStringList * game_to_txt_records(boost::shared_ptr<ServedGame const> game) {
  /* start with an empty list */
  AvahiStringList *result = avahi_string_list_new("txtvers=" TXTVERS, NULL);
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
  if (group) {
    AvahiStringList *txt_records = game_to_txt_records(game);

    avahi_threaded_poll_lock(poll);

    if (avahi_client_get_state(client) != AVAHI_CLIENT_S_RUNNING) {
      Debug("Avahi client in unexpected state on update");
    }

    if (avahi_entry_group_get_state(group) != AVAHI_ENTRY_GROUP_ESTABLISHED) {
      Debug("Avahi advert in unexpected state on update.");
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

/** \brief Internal function. Do not call from outside this file.
 * 
 * This function has to be public because internal non-methods need to call
 * it, but you should not. It creates the service for Avahi to advertise,
 * from the information you have already given the ctor.
 */
void MdnsPublisher::create_services(AvahiClient *c) {
  AvahiStringList *txt_records = game_to_txt_records(game);
  int ret;

  group = avahi_entry_group_new(c, entry_group_callback, NULL);
  if (!group) {
    std::cerr << "Failed to create Avahi advert: " << avahi_strerror(avahi_client_errno(client)) << std::endl;
    goto fail;
  }

  ret = avahi_entry_group_add_service_strlst(group,
    AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
    static_cast<AvahiPublishFlags>(0),
    game_name,
    MDNS_SERVICE_TYPE, NULL, NULL,
    game->getPort(), txt_records);

  if (ret < 0) {
    std::cerr << "Failed to add service: " << avahi_strerror(ret) << std::endl;
    goto fail;
  }

  ret = avahi_entry_group_commit(group);

  if (ret < 0) {
    std::cerr << "Failed to publish Avahi advert: " << avahi_strerror(ret) << std::endl;
    goto fail;
  }

  avahi_string_list_free(txt_records);
  return;

fail:
  avahi_string_list_free(txt_records);
  avahi_threaded_poll_stop(poll);
}


/** \brief Shut off the mDNS.
 *
 * Stops the Avahi client and frees all internal memory.
 */
MdnsPublisher::~MdnsPublisher() {
  if (poll)
    avahi_threaded_poll_stop(poll);

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

