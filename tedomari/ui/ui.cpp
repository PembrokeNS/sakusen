#include "ui/ui.h"

#include "stringutils.h"
#include "ui/mapdisplay.h"

#include <fstream>

using namespace std;
using namespace __gnu_cxx;

using namespace sakusen;
using namespace tedomari::game;
using namespace tedomari::ui;

UI::UI(Region* region, ifstream& uiConf) :
  Control(0, 0, dockStyle_fill, region), activeMapDisplay(NULL),
  commandEntryBox(NULL), alertDisplay(NULL), modes(),
  mode(NULL), currentModifiers(), expectingChars(false), commandEntry(false),
  quit(false)
{
  modes["normal"] = Mode::getNormal(this);
  mode = &modes["normal"];

  /* Execute all the commands from the configuration stream */
  while (uiConf.is_open() && !uiConf.eof()) {
    String line;
    getline(uiConf, line);
    executeCommand(converter.convertNativeToUTF8(line));
  }
}

void UI::initializeControls(Game* game)
{
  /* For the moment we just make a map display control
   * TODO: make many more things */
  commandEntryBox = new CommandEntryBox(
      0, 0, dockStyle_bottom, newRegion(0, getHeight(), getWidth(), 0)
    );
  activeMapDisplay = new MapDisplay(
      0, 0, dockStyle_fill, newRegion(0, 0, getWidth(), getHeight()), game
    );
  addSubControl(commandEntryBox);
  addSubControl(activeMapDisplay);
  addLayer();
  alertDisplay =
    new AlertDisplay(0, 0, dockStyle_top, newRegion(0, 0, getWidth(), 0));
  addSubControl(alertDisplay);
  alignSubControls();
}

void UI::resize(Region* newRegion)
{
  replaceRegion(newRegion);

  alignSubControls();
  
  paint();
}

void UI::keyDown(Key k, uint16 c)
{
  if (expectingChars) {
    /* TODO: process custom key->char mappings */
    if (0 != c) {
      /* For now we assume that the chars must be being sent to the
       * commandEntryBox.  If there's anything else to which chars can be sent
       * (e.g. a chat window) then this requires more thought */
      assert(commandEntry);
      commandEntryBox->sendChar(c, this);
      return;
    }
    /* TODO: deal with other keys which should be significant in this context,
     * such as the arrow keys */
  }
  /* TODO: process modifications */
  /* TODO: process mappings */

  /* process bindings */
  hash_map<ModifiedKey, String, ModifiedKeyHash>::const_iterator binding =
    mode->getBindings().find(ModifiedKey(k, currentModifiers));
  if (binding != mode->getBindings().end()) {
    executeCommand(binding->second);
  } else {
    Debug("Unbound key: " << k);
  }
}

void UI::keyUp(Key)
{
  /* TODO */
}

void UI::executeCommand(const String& cmdName, std::list<String>& args)
{
  hash_map<String, Command, StringHash>::const_iterator cmd =
    mode->getCommands().find(cmdName);
  
  if (cmd != mode->getCommands().end()) {
    cmd->second.execute(args, this);
  } else {
    alert(Alert("Unknown command: '" + cmdName + "'"));
  }
}

void UI::executeCommand(const String& cmdString)
{
  Debug("executing command '" << cmdString << "'");
  /* TODO: smarter parsing */
  list<String> args = stringUtils_split(cmdString, " \n\t\r");
  if (args.empty()) {
    return;
  }
  String cmdName = args.front();
  args.pop_front();
  executeCommand(cmdName, args);
}

void UI::alert(const Alert& a)
{
  alertDisplay->add(a);
  /* FIXME: realigning all subcontrols is overkill */
  alignSubControls();
  paint();
}

void UI::setCommandEntry(bool on)
{
  if (on == commandEntry) {
    return;
  }

  expectingChars = commandEntry = on;
  commandEntryBox->setVisible(on);

  if (commandEntry) {
    commandEntryBox->setDesiredHeight(20 /* FIXME: Should be appropriate height for font size */);
  } else {
    commandEntryBox->setDesiredHeight(0);
  }
  alignSubControls();
  paint();
}

void UI::addAlias(
    const String& mode,
    const String& newCmd,
    const std::list<String>& oldCmd
  )
{
  if (mode == "all") {
    /* Special case: add to all modes */
    for (hash_map<String, Mode, StringHash>::iterator modeIt = modes.begin();
        modeIt != modes.end(); ++modeIt) {
      modeIt->second.addCommand(newCmd, Command(oldCmd), this);
    }
  } else {
    hash_map<String, Mode, StringHash>::iterator modeIt = modes.find(mode);
    if (modeIt == modes.end()) {
      Debug("no such mode '" << mode << "'");
      return;
    }
    modeIt->second.addCommand(newCmd, Command(oldCmd), this);
  }
}

void UI::addBinding(
    const String& mode,
    const String& keyName,
    const std::list<String>& cmd
  )
{
  ModifiedKey key(keyName);

  if (key.getKey() == K_Unknown) {
    alert(Alert("Unknown modified key name '" + keyName + "'"));
    return;
  }
  
  if (mode == "all") {
    /* Special case: add to all modes */
    for (hash_map<String, Mode, StringHash>::iterator modeIt = modes.begin();
        modeIt != modes.end(); ++modeIt) {
      modeIt->second.addBinding(key, stringUtils_join(cmd, " "));
    }
  } else {
    hash_map<String, Mode, StringHash>::iterator modeIt = modes.find(mode);
    if (modeIt == modes.end()) {
      Debug("no such mode '" << mode << "'");
      return;
    }
    modeIt->second.addBinding(key, stringUtils_join(cmd, " "));
  }
}

void UI::moveMapRelative(sint32 dx, sint32 dy)
{
  if (activeMapDisplay == NULL) {
    return;
  }
  activeMapDisplay->translate(dx, dy);
}

void UI::moveMapRelativeFrac(double dx, double dy)
{
  if (activeMapDisplay == NULL) {
    return;
  }
  moveMapRelative(
      sint32(dx*activeMapDisplay->getDexWidth()),
      sint32(dy*activeMapDisplay->getDexHeight())
    );
}

