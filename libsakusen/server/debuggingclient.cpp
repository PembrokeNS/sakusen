#include "debuggingclient.h"

namespace sakusen{
namespace server{

DebuggingClient::DebuggingClient(
    ClientId i,
    std::ostream& output
  ) :
  Client(i),
  player(),
  outputStream(output)
{
}

void DebuggingClient::queueUpdate(const Update& update)
{
  outputStream << update << std::endl;
}

}
}

