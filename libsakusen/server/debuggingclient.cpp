#include "debuggingclient.h"

namespace sakusen{
namespace server{

DebuggingClient::DebuggingClient(
    std::ostream& output) :
  Client(),
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

