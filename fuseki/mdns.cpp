#include "mdns.h"
#include <iostream>

#include <avahi-common/defs.h>
#include <avahi-common/alternative.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/timeval.h>

/** The service name we will advertise over mDNS. mDNS service names are
 * little-endian. */
#define MDNS_SERVICE_TYPE "_sakusen._tcp"

namespace fuseki {
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
    case AVAHI_ENTRY_GROUP_REGISTERING:
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
      self->create_services(c);
      break;

    case AVAHI_CLIENT_S_COLLISION:
      break;

    case AVAHI_CLIENT_FAILURE:
      std::cerr << "Client failure: " << avahi_strerror(avahi_client_errno(c)) << std::endl;
      break;

    case AVAHI_CLIENT_CONNECTING:
    case AVAHI_CLIENT_S_REGISTERING:
      break;
  }
}

/** \brief Advertise a game on mDNS.
 *
 * Advertises a game with name \p serverName running on TCP port \p portno via
 * mDNS. Uses \p out to report progress.
 *
 * \param name Must not be NULL. (Is currently unused.)
 */
MdnsPublisher::MdnsPublisher(String name, uint32 portno) : port(portno), client(NULL), poll(NULL) {
  int error = 0;

  game_name = avahi_strdup(name.c_str());
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
  }

  if (poll) {
    avahi_threaded_poll_free(poll);
    poll = NULL;
  }
}

void MdnsPublisher::create_services(AvahiClient *c) {
  AvahiEntryGroup *group = NULL;
  int ret;

  group = avahi_entry_group_new(c, entry_group_callback, NULL);
  if (!group) {
    std::cerr << "Failed to create Avahi entry group: " << avahi_strerror(avahi_client_errno(client)) << std::endl;
    goto fail;
  }

  ret = avahi_entry_group_add_service(group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, static_cast<AvahiPublishFlags>(0), game_name, MDNS_SERVICE_TYPE, NULL, NULL, port, NULL);

  if (ret < 0) {
    std::cerr << "Failed to add service: " << avahi_strerror(ret) << std::endl;
    goto fail;
  }

  ret = avahi_entry_group_commit(group);

  if (ret < 0) {
    std::cerr << "Failed to commit entry group: " << avahi_strerror(ret) << std::endl;
    goto fail;
  }

  return;

fail:
  avahi_threaded_poll_stop(poll);
}


/** \brief Shut off the mDNS.
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
  }
  if (poll) {
    avahi_threaded_poll_free(poll);
    poll = NULL;
  }
}

}
