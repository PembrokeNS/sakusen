#ifndef SAKUSEN_SERVER_PLUGINS_REPLAY_RECORDER__REPLAYRECORDERINFO_H
#define SAKUSEN_SERVER_PLUGINS_REPLAY_RECORDER__REPLAYRECORDERINFO_H

#include <sakusen/server/plugins/plugininfo.h>
#include "replayrecorder.h"

namespace sakusen {
namespace server {
namespace plugins {
namespace replay_recorder {

class ReplayRecorderInfo : public PluginInfo {
  private:
    ReplayRecorder::Ptr replayRecorder;

    void init(PluginServerInterface& server_face);
    void close();
};

extern "C" {
  SAKUSEN_SERVER_PLUGINS_REPLAY_RECORDER_API PluginInfo*
    get_replay_recorder_info();
}

}}}}

#endif // SAKUSEN_SERVER_PLUGINS_REPLAY_RECORDER__REPLAYRECORDERINFO_H

