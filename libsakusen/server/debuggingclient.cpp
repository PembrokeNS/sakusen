#include "debuggingclient.h"

namespace sakusen{
namespace server{

DebuggingClient::DebuggingClient(
    ClientID i,
    std::ostream& output
  ) :
  Client(i),
  player(0),
  outputStream(output)
{
}

void DebuggingClient::queueUpdate(const Update& update)
{
  outputStream << update << std::endl;
}

}
}

