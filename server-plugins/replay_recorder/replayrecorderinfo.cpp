#include "replayrecorderinfo.h"

#include <sakusen/server/plugins/pluginexn.h>

namespace sakusen {
namespace server {
namespace plugins {
namespace replay_recorder {

void ReplayRecorderInfo::init(PluginServerInterface& server_face)
{
  if (replayRecorder) {
    throw PluginExn("replay_recorder already initialized");
  }
  if (server_face.isGameStarted()) {
    throw PluginExn("Must initialize replay_recorder plugin before game start");
  }

  replayRecorder.reset(new ReplayRecorder(server_face));
  server_face.registerListener(replayRecorder);
}

void ReplayRecorderInfo::close()
{
  replayRecorder.reset();
}

extern "C" {
  PluginInfo* get_replay_recorder_info()
  {
    return new ReplayRecorderInfo();
  }
}

}}}}

