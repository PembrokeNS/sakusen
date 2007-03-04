#include "replayrecorderinfo.h"

#include "pluginexn.h"

namespace sakusen {
namespace server {
namespace plugins {
namespace replay_recorder {

void ReplayRecorderInfo::init(PluginServerInterface& interface)
{
  if (replayRecorder) {
    throw PluginExn("replay_recorder already initialized");
  }
  if (interface.isGameStarted()) {
    throw PluginExn("Must initialize replay_recorder plugin before game start");
  }

  replayRecorder.reset(new ReplayRecorder(interface));
  interface.registerListener(replayRecorder);
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

