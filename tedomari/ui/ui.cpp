#include "ui/ui.h"

#include "stringutils.h"
#include "socketexn.h"
#include "ui/mapdisplay.h"
#include "ui/modifiedkeyevent.h"

#include <fstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/spirit/home/qi/parse.hpp>
#include <boost/spirit/home/qi/numeric/uint.hpp>
#include <pcrecpp.h>

using namespace std;

using namespace sakusen;
using namespace sakusen::comms;
using namespace sakusen::client;
using namespace tedomari::game;
using namespace tedomari::ui;

UI::UI(tedomari::ui::Region* region, ifstream& uiConf, Game* g) :
  Control(0, 0, dockStyle_fill, region),
  game(g),
  activeMapDisplay(NULL),
  commandEntryBox(NULL),
  statusSummary(NULL),
  alertDisplay(NULL),
  modes(),
  pendingMode(NULL),
  mode(NULL),
  currentModifiers(),
  expectingChars(false),
  commandEntry(false),
  regexEntry(false),
  quit(false)
{
  modes["normal"] = Mode::getNormal(this);
  modes["unit"] =   Mode::getUnit(this);
  modes["target"] = Mode::getTarget(this);
  modes["string"] = Mode::getDefault("string", this);
  modes["targetframe"] = Mode::getDefault("targetframe", this);
  modes["targetunit"] = Mode::getDefault("targetunit", this);
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
      "\\s*([+-]?[a-zA-Z_][a-zA-Z_0-9]*|-?[0-9\\.]+|'[^']*'|{|}|;)",
      pcrecpp::UTF8()
    );
  list<String> tokens;
  String token;
  
  while(re.Consume(&piece, &token)) {
    assert(!token.empty());
    /*SAKUSEN_QDEBUG("adding token '" << token << "'");*/
    tokens.push_back(token);
  }

  pcrecpp::RE("\\s*(?:\\#[^\\n]*)?").Consume(&piece);

  if (!piece.empty()) {
    alert(Alert(String("Unexpected character '") + piece[0] + "' in command"));
  }

  return tokens;
}

void UI::setMode(Mode* newMode)
{
  assert(newMode != NULL);
  mode = newMode;
  String indicator = "-- " + boost::to_upper_copy(mode->getName());
  if (pendingMode) {
    indicator += " (" + pendingMode->getName() + ")";
  }
  indicator += " --";
  modeIndicator->setText(indicator);
}

void UI::initializeControls()
{
  /** \todo Make many more things.
   */
  commandEntryBox = new CommandEntryBox(
      0, 0, dockStyle_bottom, newRegion(0, getHeight(), getWidth(), 0)
    );
  regexEntryBox = new RegexEntryBox(
      0, 0, dockStyle_bottom, newRegion(0, getHeight(), getWidth(), 0)
    );
  modeIndicator = new Label(
      /** \bug 20 should be appropriate height for font size */
      0, 0, dockStyle_bottom, newRegion(0, getHeight()-20, getWidth(), 20),
      "-- NORMAL --", Colour::white, Colour::black
    );
  activeMapDisplay = new MapDisplay(
      0, 0, dockStyle_fill, newRegion(0, 0, getWidth(), getHeight()), game,
      this, game->getDexPerPixel()
    );
  addSubControl(commandEntryBox);
  addSubControl(regexEntryBox);
  addSubControl(modeIndicator);
  addSubControl(activeMapDisplay);
  addLayer();
  statusSummary = new Label(
      /** \bug 20 should be appropriate height for font size */
      0, 0, dockStyle_top, newRegion(0, 0, getWidth(), 20),
      "", Colour::white, Colour(1, 0.5, 0.5, 0.5)
    );
  alertDisplay =
    new AlertDisplay(0, 0, dockStyle_top, newRegion(0, 0, getWidth(), 0));
  addSubControl(statusSummary);
  addSubControl(alertDisplay);
  alignSubControls();
}

void UI::resize(tedomari::ui::Region* newRegion)
{
  replaceRegion(newRegion);

  alignSubControls();
  
  paint();
}

void UI::keyDown(Key k, uint16 c)
{
  if (expectingChars) {
    /** \todo Process custom key->char mappings */
    if (0 != c) {
      /* For now we assume that the chars must be being sent to the
       * commandEntryBox.  If there's anything else to which chars can be sent
       * (e.g. a chat window) then this requires more thought */
      assert(commandEntry || regexEntry);
      if (commandEntry) {
        commandEntryBox->sendChar(c, this);
      } else if (regexEntry) {
        regexEntryBox->sendChar(c, this);
      }
      return;
    }
    /** \todo Deal with other keys which should be significant in this context,
     * such as the arrow keys */
  }
  /** \todo Process modifications */
  /** \todo Process mappings */

  /* Process bindings */
  u_map<ModifiedKeyEvent, list<String>, ModifiedKeyEventHash>::type::
    const_iterator binding =
    mode->getBindings().find(ModifiedKeyEvent(k, currentModifiers, true));
  if (binding != mode->getBindings().end()) {
    list<String> tokens(binding->second);
    executeCommands(tokens);
  } else {
    SAKUSEN_DEBUG("Unbound key: +" << getName(k));
  }
}

void UI::keyUp(Key k)
{
  /** \todo Process modifications */
  /** \todo Process mappings */

  /* process bindings */
  u_map<ModifiedKeyEvent, list<String>, ModifiedKeyEventHash>::type::
    const_iterator binding =
    mode->getBindings().find(ModifiedKeyEvent(k, currentModifiers, false));
  if (binding != mode->getBindings().end()) {
    list<String> tokens(binding->second);
    executeCommands(tokens);
  } else {
    /*SAKUSEN_DEBUG("Unbound key: -" << getName(k));*/
  }
}

/** \brief Set mode appropriate for producing given ActionParameterType */
void UI::setModeFor(ActionParameterType type)
{
  /* When this assertion fails, update the switch appropriately */
  BOOST_MPL_ASSERT_RELATION(actionParameterType_max,==,7);
  switch (type) {
    case actionParameterType_none:
      if (pendingMode) {
        Mode* tmpMode = pendingMode;
        pendingMode = NULL;
        setMode(tmpMode);
      }
      break;
    case actionParameterType_target:
      setMode(&modes["target"]);
      break;
    case actionParameterType_stringFromSet:
    case actionParameterType_number:
      setMode(&modes["string"]);
      break;
    case actionParameterType_frame:
      setMode(&modes["targetframe"]);
      break;
    case actionParameterType_unit:
    case actionParameterType_unitSet:
      setMode(&modes["targetunit"]);
      break;
    default:
      SAKUSEN_FATAL("unexpected ActionParameterType " << type);
  }
}

vector<Ref<UpdatedUnit> > UI::getUnitsAtCursor() {
  vector<Ref<UpdatedUnit> > result;

  Point<sint32> cursorPos = activeMapDisplay->getMousePosAtZero();
  Rectangle<sint32> cursorRect(cursorPos, cursorPos);
  ISpatial::Result units =
    client::world->getSpatialIndex()->findIntersecting(
        cursorRect, gameObject_unit
      );

  for (ISpatial::Result::iterator unitIt = units.begin();
      unitIt != units.end(); ++unitIt) {
    result.push_back(unitIt->dynamicCast<UpdatedUnit>());
  }
  return result;
}

void UI::update()
{
  /** \todo Should call all MapDisplays, once there can be more than one */
  if (game->isDirty()) {
    game->clearDirty();
    activeMapDisplay->update();
    ostringstream status;
    status <<
      "E: " << game->getAvailableEnergy() <<
      " M: " << game->getAvailableMetal();
    statusSummary->setText(status.str());
  }
}

void UI::executeCommand(const String& cmdName, std::list<String>& args)
{
  SAKUSEN_QDEBUG(
      "executing '" << cmdName << "' with args '" <<
      stringUtils_join(args, "' '") << "'"
    );
  u_map<String, Command>::type::const_iterator cmd =
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
    /* Strip quotes from token */
    if (!token->empty() && (*token)[0] == '\'') {
      token->erase(token->begin());
      assert(!token->empty());
      assert(*(token->end()-1) == '\'');
      token->erase(token->end()-1);
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
  /*SAKUSEN_DEBUG("executing command '" << cmdString << "'");*/
  list<String> tokens = tokenise(cmdString);
  if (tokens.empty()) {
    return;
  }
  executeCommands(tokens);
}

void UI::executeRegex(const String& regex)
{
  pcrecpp::RE r(regex);
  /** \todo Other possible uses of regexes */
  if (pendingAction) {
    switch (pendingAction->getNextParameterType()) {
      case actionParameterType_stringFromSet:
        {
          const set<String>& options = pendingAction->getStringSet();
          for (set<String>::const_iterator i=options.begin();
              i != options.end(); ++i ) {
            if (r.PartialMatch(*i)) {
              supplyActionArg(ActionArgument(*i));
              return;
            }
          }
          String options_s;
          if (options.empty()) {
            options_s = "no valid options";
          } else {
            options_s = "options were " + boost::algorithm::join(options, ", ");
          }
          alert("No option matched regex '"+regex+"' ("+options_s+")");
          return;
        }
      case actionParameterType_number:
        {
          NumberTarget val;
          std::string::const_iterator st = regex.begin();
          namespace qi = boost::spirit::qi;
          qi::uint_parser<NumberTarget, 10, 1, -1> parser;
          if (qi::parse(st, regex.end(), parser, val) && st == regex.end()) {
            supplyActionArg(ActionArgument(val));
          } else {
            alert("Couldn't interpret '"+regex+"' as a number");
          }
          return;
        }
      default:
        alert("No obvious purpose for regex");
    }
  }
}

void UI::alert(const Alert& a)
{
  if (alertDisplay == NULL) {
    SAKUSEN_QDEBUG("Alert: " + a.getMessage());
  } else {
    alertDisplay->add(a);
    /** \bug Realigning all subcontrols is overkill */
    alignSubControls();
    paint();
  }
}

void UI::setCommandEntry(bool on)
{
  if (on == commandEntry || commandEntryBox == NULL) {
    return;
  }

  if (on && regexEntry) {
    setRegexEntry(false);
  }

  expectingChars = commandEntry = on;
  commandEntryBox->setVisible(on);

  if (commandEntry) {
    commandEntryBox->setDesiredHeight(20
        /** \bug Should be appropriate height for font size */);
  } else {
    commandEntryBox->setDesiredHeight(0);
  }
  alignSubControls();
  paint();
}

void UI::setRegexEntry(bool on)
{
  if (on == regexEntry || regexEntryBox == NULL) {
    return;
  }

  if (on && commandEntry) {
    setCommandEntry(false);
  }

  expectingChars = regexEntry = on;
  regexEntryBox->setVisible(on);

  if (regexEntry) {
    regexEntryBox->setDesiredHeight(20
        /** \bug Should be appropriate height for font size */);
  } else {
    regexEntryBox->setDesiredHeight(0);
  }
  alignSubControls();
  paint();
}

void UI::switchMode(const String& modeName)
{
  u_map<String, Mode>::type::iterator modeIt = modes.find(modeName);
  if (modeIt == modes.end()) {
    alert(Alert("No such mode '" + modeName + "'"));
    return;
  }
  pendingMode = NULL;
  setMode(&modeIt->second);
}

void UI::addAlias(
    const String& mode,
    const String& newCmd,
    const std::list<String>& oldCmd
  )
{
  if (mode == "all") {
    /* Special case: add to all modes */
    for (u_map<String, Mode>::type::iterator modeIt = modes.begin();
        modeIt != modes.end(); ++modeIt) {
      modeIt->second.addCommand(newCmd, Command(oldCmd), this);
    }
  } else {
    u_map<String, Mode>::type::iterator modeIt = modes.find(mode);
    if (modeIt == modes.end()) {
      SAKUSEN_DEBUG("no such mode '" << mode << "'");
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
    for (u_map<String, Mode>::type::iterator modeIt = modes.begin();
        modeIt != modes.end(); ++modeIt) {
      modeIt->second.addBinding(keyEvent, cmd);
    }
  } else {
    u_map<String, Mode>::type::iterator modeIt = modes.find(mode);
    if (modeIt == modes.end()) {
      SAKUSEN_DEBUG("no such mode '" << mode << "'");
      return;
    }
    modeIt->second.addBinding(keyEvent, cmd);
  }
}

void UI::addFunction(const String& mode, const Function& function)
{
  if (mode == "all") {
    /* Special case: add to all modes */
    for (u_map<String, Mode>::type::iterator modeIt = modes.begin();
        modeIt != modes.end(); ++modeIt) {
      modeIt->second.addFunction(function, this);
    }
  } else {
    u_map<String, Mode>::type::iterator modeIt = modes.find(mode);
    if (modeIt == modes.end()) {
      SAKUSEN_DEBUG("no such mode '" << mode << "'");
      return;
    }
    modeIt->second.addFunction(function, this);
  }
}

void UI::initializeAction(const String& actionName)
{
  pendingAction = ::initializeAction(actionName, selection);
  if (!pendingAction) {
    alert(Alert("Unknown action "+actionName));
    return;
  }
  if (pendingMode == NULL) {
    pendingMode = mode;
  }
  if (pendingAction->getNextParameterType() == actionParameterType_none) {
    pendingAction->execute(this);
  }
  setModeFor(pendingAction->getNextParameterType());
}

void UI::abortAction()
{
  pendingAction.reset();
  setModeFor(actionParameterType_none);
}

void UI::supplyActionArg(const String& actionArg)
{
  /* For now we just support the string 'cursor', meaning cursor position */
  if (actionArg == "cursor") {
    switch (pendingAction->getNextParameterType()) {
      case actionParameterType_target:
        supplyActionArg(activeMapDisplay->getMousePosAtGround());
        break;
      case actionParameterType_frame:
        {
          Position pos = activeMapDisplay->getMousePosAtGround();
          pos.z += pendingAction->getSize().z;
          /** \todo Support other orientations */
          supplyActionArg(Frame(pos, Orientation()));
        }
        break;
      case actionParameterType_unit:
        {
          vector<Ref<UpdatedUnit> > units = getUnitsAtCursor();
          if (!units.empty()) {
            supplyActionArg(units.back());
          }
        }
        break;
      case actionParameterType_unitSet:
        {
          vector<Ref<UpdatedUnit> > units = getUnitsAtCursor();
          if (!units.empty()) {
            supplyActionArg(units);
          }
        }
        break;
      default:
        {
          ostringstream s;
          s << pendingAction->getNextParameterType();
          alert(Alert("'cursor' argument invalid (need "+s.str()+")"));
        }
    }
  } else {
    alert(Alert("Unrecognized action argument '"+actionArg+"'"));
  }
}

void UI::supplyActionArg(const ActionArgument& actionArg)
{
  if (!pendingAction) {
    alert(Alert("No pending action for argument"));
    return;
  }
  try {
    pendingAction->supplyArgument(actionArg);
    if (pendingAction->getNextParameterType() == actionParameterType_none) {
      pendingAction->execute(this);
    }
    setModeFor(pendingAction->getNextParameterType());
  } catch (SocketExn& e) {
    alert("Caught SocketExn ("+e.message+")!");
    /** \bug Need to do something more dramatic at this point */
  } catch (Exn& e) {
    alert(Alert("Problem with argument: "+e.message));
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
  /** \todo This string should be interpretable in more ways */
  if (selection == "lastrectangle") {
    selectUnitsIn(lastRectangle);
    return;
  }

  alert(Alert("Unrecognized selection '" + selection + "'"));
}

void UI::selectUnitsIn(const sakusen::Rectangle<sint32>& r)
{
  ISpatial::Result units =
    client::world->getSpatialIndex()->findIntersecting(r, gameObject_unit);
  selection.clear();
  for (ISpatial::Result::iterator unitIt = units.begin();
      unitIt != units.end(); ++unitIt) {
    Ref<UpdatedUnit> unit = unitIt->dynamicCast<UpdatedUnit>();
    selection.insert(unit->getId());
  }
  paint();
}

void UI::stop(const set<sakusen::UnitId>& units)
{
  Order stopMoveOrder = Order(new SetVelocityOrderData(Velocity()));
  Order stopRotationOrder = Order(new SetAngularVelocityOrderData(AngularVelocity()));

  BOOST_FOREACH (UnitId unitId, units) {
    Ref<UpdatedUnit> unit = client::world->getUnitsById()->find(unitId);
    if (unit) {
      game->order(OrderMessage(unitId, stopMoveOrder));
      game->order(OrderMessage(unitId, stopRotationOrder));

      const UnitType* type =
        client::world->getUniverse()->getUnitTypePtr(unit->getStatus().getType());
      /* For every weapon, send a stop order for that weapon */
      size_t numWeapons = type->getWeapons().size();

      for (size_t i=0; i<numWeapons; ++i) {
        Order order(new TargetNoneOrderData(i));
        game->order(OrderMessage(unitId, order));
      }
    }
  }
}

/** \internal \brief Helper class to foward move calls
 *
 * This class is constructed with the set of units to move, and then when it is
 * used as a visitor on an ActionTarget, it orders the units to move to that
 * target.
 */
class MoveVisitor : public boost::static_visitor<void> {
  public:
    /** \warning Stores a reference to its second argument */
    MoveVisitor(UI* u, const set<UnitId>& un) :
      ui(u), units(un)
    {}
  private:
    UI* ui;
    const set<UnitId>& units;
  public:
    /** \brief Orders units to move to target
     *
     * \param t Target to which to move units */
    template<typename T>
    void operator()(const T& t) {
      ui->move(units, t);
    }
};

/** \todo Replace this method with the order-queue related things */
void UI::move(const set<sakusen::UnitId>& units, const ActionTarget& target)
{
  MoveVisitor moveVisitor(this, units);
  target.apply_visitor(moveVisitor);
}

void UI::move(const std::set<sakusen::UnitId>& units, const Position& target)
{
  const Map* map = sakusen::world->getMap();
  Order moveOrder = Order(new MoveOrderData(target));
  
  BOOST_FOREACH (UnitId unitId, units) {
    Ref<UpdatedUnit> unit = client::world->getUnitsById()->find(unitId);
    if (unit) {
      game->order(OrderMessage(unitId, moveOrder));
      const Point<sint32> displacement =
        map->getShortestDifference(
            target, unit->getStatus().getFrame().getPosition()
          );
      const double angle = atan2(static_cast<double>(displacement.y), displacement.x);
      const Orientation desiredOrientation(
          AngularVelocity(0, 0, AngularVelocity::element_type(angle*18000/M_PI))
        );
      const Order rotationOrder(new OrientOrderData(desiredOrientation));
      game->order(OrderMessage(unitId, rotationOrder));
    }
  }
}

