#ifndef SAKUSEN_SERVER_PLUGINS_REPLAY_RECORDER__REPLAYRECORDER_H
#define SAKUSEN_SERVER_PLUGINS_REPLAY_RECORDER__REPLAYRECORDER_H

#include <sakusen/global.h>
#include <sakusen/server/plugins/listener.h>
#include <sakusen/server/plugins/pluginserverinterface.h>
#include <sakusen/server/plugins/optioncallback.h>
#include <sakusen/clientid.h>

/* Exporting functions. This should never be an import library,
 * so need not use dllimport. */
#ifdef _MSC_VER
  #define SAKUSEN_SERVER_PLUGINS_REPLAY_RECORDER_API __declspec(dllexport)
#else
  #if (__GNUC__ >= 4)
    #define SAKUSEN_SERVER_PLUGINS_REPLAY_RECORDER_API \
      __attribute__ ((visibility ("default")))
  #else
    #define SAKUSEN_SERVER_PLUGINS_REPLAY_RECORDER_API
  #endif
#endif 

namespace sakusen {
namespace server {
namespace plugins {
namespace replay_recorder {

/** \brief Listener which listens to everything and stores it on disk in the
 * form of a replay.
 *
 */
class ReplayRecorder :
  public Listener,
  private OptionCallback<String>,
  private OptionCallback<uint16> {
  public:
    ReplayRecorder(const PluginServerInterface&);
  private:
    std::vector<std::pair<ClientId, comms::Message> > thisTickMessages;
    String replayFileRoot;
    uint16 keyTickInterval;
    ResourceInterface::Ptr resourceInterface;
    Writer::Ptr mainReplayWriter;
    Writer::Ptr replayIndexWriter;

    void writeKeyTick();
  protected:
    String setOption(const String& name, const String& newVal);
    String setOption(const String& name, const uint16 newVal);
    void gameStart();
    void clientMessage(const Client*, const comms::Message&);
    void ticksMessagesDone();
    void tickDone();
};

}}}}

#endif // SAKUSEN_SERVER_PLUGINS_REPLAY_RECORDER__REPLAYRECORDER_H

