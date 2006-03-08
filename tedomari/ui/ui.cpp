#include "ui/ui.h"

#include "stringutils.h"
#include "ui/mapdisplay.h"
#include "ui/modifiedkeyevent.h"

#include <fstream>
#include <pcrecpp.h>

using namespace std;
using namespace __gnu_cxx;

using namespace sakusen;
using namespace sakusen::client;
using namespace tedomari::game;
using namespace tedomari::ui;

UI::UI(Region* region, ifstream& uiConf) :
  Control(0, 0, dockStyle_fill, region), activeMapDisplay(NULL),
  commandEntryBox(NULL), alertDisplay(NULL), modes(),
  mode(NULL), currentModifiers(), expectingChars(false), commandEntry(false),
  quit(false)
{
  modes["normal"] = Mode::getNormal(this);
  modes["unit"] =   Mode::getUnit(this);
  mode = &modes["normal"];

  list<String> tokens;
  uint16 depth = 0;
  
  /* Execute all the commands from the configuration stream */
  while (uiConf.is_open() && !uiConf.eof()) {
    String line;
    getline(uiConf, line);
    list<String> newTokens(tokenise(converter.convertNativeToUTF8(line)));
    for (list<String>::iterator token = newTokens.begin();
        token != newTokens.end(); ++token) {
      if (*token == "{") {
        ++depth;
      } else if (*token == "}") {
        if (0 == depth) {
          alert(Alert("Unexpected '}' in configuration file"));
        } else {
          --depth;
        }
      }
    }
    tokens.splice(tokens.end(), newTokens);
    if (depth == 0) {
      executeCommands(tokens);
      tokens.clear();
    }
  }

  if (0 != depth) {
    alert(Alert("Expected '}' at end of configuration file"));
  }
}

list<String> UI::tokenise(const String& s)
{
  pcrecpp::StringPiece piece(s);
  pcrecpp::RE re(
      "\\s*([+-]?[a-zA-Z_]+|-?[0-9\\.]+|'[^']*'|{|}|;)", pcrecpp::UTF8()
    );
  list<String> tokens;
  String token;
  
  while(re.Consume(&piece, &token)) {
    assert(!token.empty());
    /*QDebug("adding token '" << token << "'");*/
    tokens.push_back(token);
  }

  pcrecpp::RE("\\s+").Consume(&piece);

  if (!piece.empty()) {
    alert(Alert(String("Unexpected character '") + piece[0] + "' in command"));
  }

  return tokens;
}

void UI::initializeControls(Game* game)
{
  /* For the moment we just make a map display control
   * TODO: make many more things */
  commandEntryBox = new CommandEntryBox(
      0, 0, dockStyle_bottom, newRegion(0, getHeight(), getWidth(), 0)
    );
  activeMapDisplay = new MapDisplay(
      0, 0, dockStyle_fill, newRegion(0, 0, getWidth(), getHeight()), game,
      this
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
  hash_map<ModifiedKeyEvent, list<String>, ModifiedKeyEventHash>::
    const_iterator binding =
    mode->getBindings().find(ModifiedKeyEvent(k, currentModifiers, true));
  if (binding != mode->getBindings().end()) {
    list<String> tokens(binding->second);
    executeCommands(tokens);
  } else {
    Debug("Unbound key: +" << getName(k));
  }
}

void UI::keyUp(Key k)
{
  /* TODO: process modifications */
  /* TODO: process mappings */

  /* process bindings */
  hash_map<ModifiedKeyEvent, list<String>, ModifiedKeyEventHash>::
    const_iterator binding =
    mode->getBindings().find(ModifiedKeyEvent(k, currentModifiers, false));
  if (binding != mode->getBindings().end()) {
    list<String> tokens(binding->second);
    executeCommands(tokens);
  } else {
    Debug("Unbound key: -" << getName(k));
  }
}

void UI::update()
{
  /* TODO: should call all MapDisplays, once there can be more than one */
  activeMapDisplay->update();
}

void UI::executeCommand(const String& cmdName, std::list<String>& args)
{
  QDebug(
      "executing '" << cmdName << "' with args '" <<
      stringUtils_join(args, "' '") << "'"
    );
  hash_map<String, Command, StringHash>::const_iterator cmd =
    mode->getCommands().find(cmdName);
  
  if (cmd != mode->getCommands().end()) {
    cmd->second.execute(args, this);
  } else {
    alert(Alert("Unknown command: '" + cmdName + "'"));
  }
}

void UI::executeCommand(std::list<String>& tokens)
{
  for (list<String>::iterator token = tokens.begin(); token != tokens.end();
      ++token) {
    assert(!token->empty());
    /* Strip quotes from token */
    if ((*token)[0] == '\'') {
      token->erase(token->begin());
      assert(!token->empty());
      assert(*--token->end() == '\'');
      token->erase(--token->end());
    }
  }
    
  String cmdName = tokens.front();
  tokens.pop_front();
  executeCommand(cmdName, tokens);
}

void UI::executeCommands(list<String>& tokens)
{
  /* We search through the list for special tokens (';', '{', '}') to help us
   * split the list into commands */
  uint16 depth = 0;
  list<String>::iterator stop = tokens.begin();

  while (stop != tokens.end()) {
    if (*stop == "{") {
      ++depth;
      ++stop;
      continue;
    }
    if (*stop == "}") {
      if (depth > 0) {
        --depth;
        ++stop;
        continue;
      } else {
        alert(Alert("Too many close braces"));
        return;
      }
    }
    if (*stop == ";") {
      if (stop == tokens.begin()) {
        ++stop;
        continue;
      }
      if (depth == 0) {
        list<String> cmdTokens;
        cmdTokens.splice(cmdTokens.end(), tokens, tokens.begin(), stop);
        executeCommand(cmdTokens);
        ++stop;
        tokens.pop_front();
        continue;
      }
    }
    ++stop;
  }

  if (0 != depth) {
    alert(Alert("Expected '}' at end of command"));
    return;
  }

  if (!tokens.empty()) {
    executeCommand(tokens);
  }
}

void UI::executeCommands(const String& cmdString)
{
  /*Debug("executing command '" << cmdString << "'");*/
  /* TODO: smarter parsing */
  list<String> tokens = tokenise(cmdString);
  if (tokens.empty()) {
    return;
  }
  executeCommands(tokens);
}

void UI::alert(const Alert& a)
{
  if (alertDisplay == NULL) {
    QDebug("Alert: " + a.getMessage());
  } else {
    alertDisplay->add(a);
    /* FIXME: realigning all subcontrols is overkill */
    alignSubControls();
    paint();
  }
}

void UI::setCommandEntry(bool on)
{
  if (on == commandEntry || commandEntryBox == NULL) {
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

void UI::switchMode(const String& modeName)
{
  hash_map<String, Mode, StringHash>::iterator modeIt = modes.find(modeName);
  if (modeIt == modes.end()) {
    alert(Alert("No such mode '" + modeName + "'"));
    return;
  }
  mode = &modeIt->second;
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
  ModifiedKeyEvent keyEvent(keyName);

  if (keyEvent.getModifiedKey().getKey() == K_Unknown) {
    alert(Alert("Unknown modified key name '" + keyName + "'"));
    return;
  }
  
  if (mode == "all") {
    /* Special case: add to all modes */
    for (hash_map<String, Mode, StringHash>::iterator modeIt = modes.begin();
        modeIt != modes.end(); ++modeIt) {
      modeIt->second.addBinding(keyEvent, cmd);
    }
  } else {
    hash_map<String, Mode, StringHash>::iterator modeIt = modes.find(mode);
    if (modeIt == modes.end()) {
      Debug("no such mode '" << mode << "'");
      return;
    }
    modeIt->second.addBinding(keyEvent, cmd);
  }
}

void UI::addFunction(const String& mode, const Function& function)
{
  if (mode == "all") {
    /* Special case: add to all modes */
    for (hash_map<String, Mode, StringHash>::iterator modeIt = modes.begin();
        modeIt != modes.end(); ++modeIt) {
      modeIt->second.addFunction(function, this);
    }
  } else {
    hash_map<String, Mode, StringHash>::iterator modeIt = modes.find(mode);
    if (modeIt == modes.end()) {
      Debug("no such mode '" << mode << "'");
      return;
    }
    modeIt->second.addFunction(function, this);
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

void UI::dragRegion(bool start)
{
  if (start) {
    activeMapDisplay->startDrag();
  } else {
    lastRectangle = activeMapDisplay->stopDrag();
  }
}

void UI::selectUnits(const String& selection)
{
  /* TODO: this string should be interpretable in more ways */
  if (selection == "lastrectangle") {
    selectUnitsIn(lastRectangle);
    return;
  }

  alert(Alert("Unrecognized selection '" + selection + "'"));
}

void UI::selectUnitsIn(const sakusen::Rectangle<sint32>& r)
{
  list<UpdatedUnit*> units(sakusen::client::world->getUnitsIntersecting(r));
  selection.clear();
  selection.insert(units.begin(), units.end());
  paint();
}

