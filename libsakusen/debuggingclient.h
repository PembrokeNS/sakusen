#ifndef DEBUGGINGCLIENT_H
#define DEBUGGINGCLIENT_H

#include "libsakusen-global.h"

#include <iosfwd>

#include "client.h"
#include "player.h"

/* This class provides a debugging client which dumps information to
 * a stream about what messages it receives, etc. */

namespace sakusen {

class LIBSAKUSEN_API DebuggingClient : public Client {
  public:
    DebuggingClient();
    DebuggingClient(std::ostream& output);
    ~DebuggingClient() {};
  private:
    PlayerID player;
    std::ostream& outputStream;
  public:
    void sendUpdate(const Update& update);
};

}

#endif // DEBUGGINGCLIENT_H

