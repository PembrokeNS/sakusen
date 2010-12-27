#include "replayrecorder.h"

#include <boost/xpressive/regex_algorithms.hpp>

#include <sakusen/resources/global.h>
#include <sakusen/oarchive-methods.h>
#include <sakusen/server/plugins/pluginexn.h>

#include <sakusen/server/completeworld.h>

using namespace std;

using namespace sakusen::comms;

namespace sakusen {
namespace server {
namespace plugins {
namespace replay_recorder {

ReplayRecorder::ReplayRecorder(const PluginServerInterface& interface) :
  keyTickInterval(0),
  resourceInterface(interface.getResourceInterface())
{
  interface.registerOption("file", this, "");
  interface.registerOption("keytickinterval", this, keyTickInterval);
}

void ReplayRecorder::writeKeyTick()
{
  if (!mainReplayWriter)
    return;
  
  OArchive keyTick;

  /* 1 indicates that what follows is an entire copy of the gamestate */
  keyTick.magicValue("REPLAY") << uint8(1);
  world->save(keyTick);

  /* We also need to store in the index where we put this in the replay file,
   * for seeking */
  uint64 keyTickStart = mainReplayWriter->tell();
  
  mainReplayWriter->write(keyTick);

  OArchive indexEntry;
  indexEntry.magicValue("I") << world->getTimeNow() << keyTickStart;

  replayIndexWriter->write(indexEntry);
}

String ReplayRecorder::setOption(const String& name, const String& newVal)
{
  if (name == "file") {
    if (!(newVal == "" || regex_match(newVal, resources::filenameRegex))) {
      return "filename contains invalid characters";
    }
    replayFileRoot = newVal;
    return "";
  } else {
    SAKUSEN_FATAL("unexpected string option name " << name);
  }
}

String ReplayRecorder::setOption(const String& name, const uint16 newVal)
{
  if (name == "keytickinterval") {
    keyTickInterval = newVal;
    return "";
  } else {
    SAKUSEN_FATAL("unexpected string option name " << name);
  }
}

void ReplayRecorder::gameStart()
{
  if (replayFileRoot == "") {
    SAKUSEN_QDEBUG("ReplayRecorder not recording because no filename specified");
    return;
  }
  
  /* Open the writers */
  mainReplayWriter =
    resourceInterface->openWriter("replays/"+replayFileRoot+".sakusenreplay");
  replayIndexWriter =
    resourceInterface->openWriter("replays/"+replayFileRoot+".sakusenreplayindex");

  /* Save a keytick */
  writeKeyTick();
}

void ReplayRecorder::clientMessage(const Client* client, const Message& message)
{
  thisTickMessages.push_back(make_pair(client->getClientId(), message));
}

void ReplayRecorder::ticksMessagesDone()
{
  if (!mainReplayWriter) {
    return;
  }
  
  OArchive ticksMessages;

  /* 0 indicates that what follows is a ticks worth of messages */
  ticksMessages.magicValue("REPLAY") << uint8(0) << world->getTimeNow() <<
    thisTickMessages;
  mainReplayWriter->write(ticksMessages);
}

void ReplayRecorder::tickDone()
{
  if (keyTickInterval && 0 == world->getTimeNow() % keyTickInterval)
  {
    /* This means we should save a keytick */
    writeKeyTick();
  }
}

}}}}

