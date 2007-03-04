#ifndef REPLAYRECORDERINFO_H
#define REPLAYRECORDERINFO_H

#include "plugininfo.h"
#include "replayrecorder.h"

namespace sakusen {
namespace server {
namespace plugins {
namespace replay_recorder {

class ReplayRecorderInfo : public PluginInfo {
  private:
    ReplayRecorder::Ptr replayRecorder;

    void init(PluginServerInterface& interface);
    void close();
};

extern "C" {
  PluginInfo* get_replay_recorder_info();
}

}}}}

#endif // REPLAYRECORDERINFO_H

