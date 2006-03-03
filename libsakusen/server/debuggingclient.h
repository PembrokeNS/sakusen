#ifndef LIBSAKUSEN_SERVER__DEBUGGINGCLIENT_H
#define LIBSAKUSEN_SERVER__DEBUGGINGCLIENT_H

#include "libsakusen-global.h"

#include <iosfwd>

#include "client.h"
#include "player.h"

/* This class provides a debugging client which dumps information to
 * a stream about what messages it receives, etc. */

namespace sakusen {
namespace server {

class LIBSAKUSEN_API DebuggingClient : public Client {
  public:
    DebuggingClient();
    DebuggingClient(std::ostream& output);
    ~DebuggingClient() {};
  private:
    PlayerID player;
    std::ostream& outputStream;
  public:
    void flushOutgoing(Time) {}
    void queueUpdate(const Update& update);
};

}}

#endif // LIBSAKUSEN_SERVER__DEBUGGINGCLIENT_H

