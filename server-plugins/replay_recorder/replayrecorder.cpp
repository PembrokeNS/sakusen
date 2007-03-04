#include "replayrecorder.h"

#include "libsakusen-resources-global.h"
#include "oarchive-methods.h"
#include "pluginexn.h"

#include "completeworld.h"

#include <pcrecpp.h>

using namespace std;

using namespace pcrecpp;

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
  OArchive keyTick;

  /* 1 indicates that what follows is an entire copy of the gamestate */
  keyTick.magicValue("REPLAY") << uint8(1);
  world->save(keyTick);
}

String ReplayRecorder::setOption(const String& name, const String& newVal)
{
  if (name == "file") {
    if (!(newVal == "" || RE(FILENAME_REGEX).FullMatch(newVal))) {
      return "filename contains invalid characters";
    }
    replayFileRoot = newVal;
    return "";
  } else {
    Fatal("unexpected string option name " << name);
  }
}

String ReplayRecorder::setOption(const String& name, const uint16 newVal)
{
  if (name == "keytickinterval") {
    keyTickInterval = newVal;
    return "";
  } else {
    Fatal("unexpected string option name " << name);
  }
}

void ReplayRecorder::gameStart()
{
  if (replayFileRoot == "") {
    QDebug("ReplayRecorder not recording because no filename specified");
    return;
  }
  
  /* Open the writers */
  mainReplayWriter =
    resourceInterface->openWriter(replayFileRoot, resourceType_replay);
  replayIndexWriter =
    resourceInterface->openWriter(replayFileRoot, resourceType_replayIndex);

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

