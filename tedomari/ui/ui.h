#ifndef TEDOMARI__UI__UI_H
#define TEDOMARI__UI__UI_H

#include "libsakusen-global.h"

#include "stringutils.h"
#include "ptrhash.h"

#include "converter.h"
#include "game/game.h"
#include "ui/key.h"
#include "ui/control.h"
#include "ui/commandentrybox.h"
#include "ui/regexentrybox.h"
#include "ui/mode.h"
#include "ui/alert.h"
#include "ui/alertdisplay.h"
#include "ui/mapdisplay.h"
#include "ui/function.h"
#include "ui/actiontarget.h"
#include "ui/action.h"
#include "heightfield-methods.h"

namespace tedomari {
namespace ui {

class UI : protected Control {
  public:
    typedef boost::shared_ptr<UI> Ptr;
  private:
    UI();
    UI(const UI&);
  public:
    UI(Region* region, std::ifstream& uiConf, tedomari::game::Game*);
    virtual ~UI() {}
  private:
    Converter converter; /**< Converts between UTF-8 and native encodings */
    tedomari::game::Game* game;
    MapDisplay* activeMapDisplay; /**< Currently active MapDisplay, or NULL */
    CommandEntryBox* commandEntryBox;
    RegexEntryBox* regexEntryBox;
    Label* modeIndicator;
    Label* statusSummary;
    AlertDisplay* alertDisplay;
    sakusen::hash_map_string<Mode>::type modes;
    /** \brief Mode we intend to revert to when the current action executes
     * (NULL if no Action is in progress) */
    Mode* pendingMode;
    /** \brief Mode we are currently in */
    Mode* mode;
    /** \brief Modifiers like ctrl from keys currently held down */
    std::set<String> currentModifiers;
    
    /** \brief True iff we are currently interested in characters input,
     * rather than keys pressed
     *
     * For example, this will be true when currently typing a command or
     * regex, but false when waiting for keypresses which signify commands
     * */
    bool expectingChars;
    bool commandEntry;
    bool regexEntry;
    bool quit;

    sakusen::Point<double> mousePos;

    /** \brief The last selection rectangle */
    sakusen::Rectangle<sint32> lastRectangle;

    /** \brief The friendly units currently selected (by unitid) */
    std::set<sakusen::UnitId> selection;
    /** \brief The Action which is presently partly initialised (or
     * Action::Ptr() if no such Action */
    Action::Ptr pendingAction;

    std::list<String> tokenise(const String&);
    void setMode(Mode*);
  protected:
    /** \brief Create and position initial controls
     *
     * Called by a subclass (probably constructor) to create the first controls
     * and place them on the UI.
     */
    void initializeControls();

    /** \brief Create a new Region
     *
     * When overridden in a subclass, this returns a new Region object covering
     * the specified area, which can be used to draw to.  Caller is responsible
     * for deleting the returned object */
    virtual Region* newRegion(uint16 x, uint16 y, uint16 width, uint16 height)
      = 0;
    
    /** \brief Indicate window resize
     *
     * Called by a subclass to indicate that the UI window has been resized,
     * passing the new Region of the window */
    void resize(Region* region);

    virtual void resize(uint16 width, uint16 height) = 0;

    /** \brief Indicate keydown event
     *
     * Called by a subclass to indicate that a key has been pressed, passing
     * the key in question */
    void keyDown(Key, uint16 c);

    /** \brief Indicate keyup event
     *
     * Called by a subclass to indicate that a key has been released, passing
     * the key in question */
    void keyUp(Key);

    /** \brief Indicate mouse movement
     *
     * Called by a subclass to indicate that the mouse cursor has moved,
     * passing the position moved to */
    inline void mouseMove(const sakusen::Point<double>& p) {
      mousePos = p;
      activeMapDisplay->mouseMove(
          activeMapDisplay->getRegion()->globalToLocal(p)
        );
    }

    void setModeFor(ActionParameterType);
  public:
    inline const std::set<sakusen::UnitId>& getSelection() {
      return selection;
    }
    std::vector<sakusen::Ref<sakusen::client::UpdatedUnit> > getUnitsAtCursor();
    /** \brief Determine whether the user has asked to quit */
    inline bool isQuit() const { return quit; }
    inline game::Game* getGame() { return game; }
    /** \brief Repaint whole UI */
    inline void paint() { Control::paint(); }
    /** \brief Set the window title to the given String */
    virtual void setTitle(const String& title) = 0;
    /** \brief Process all user input and redraw the screen as necessary */
    virtual void update();

    /** \brief Construct and return a surface for drawing into memory */
    virtual Surface::Ptr createSurface(uint16 width, uint16 height) = 0;
    
    /** \brief Execute the given command
     *
     * \warning Modifies its argument args */
    void executeCommand(const String& cmdName, std::list<String>& args);
    void executeCommand(std::list<String>& tokens);

    void executeCommands(std::list<String>& tokens);
    void executeCommands(const String& cmd);

    void executeRegex(const String& re);
    
    /** \brief Add an alert to the list */
    void alert(const Alert&);
    inline void alert(const String s) { alert(Alert(s)); }
    
    void setCommandEntry(bool on);
    void setRegexEntry(bool on);

    /** \brief Indicate quit request */
    inline void quitRequest() { quit = true; }

    void switchMode(const String& modeName);

    void addAlias(
        const String& mode,
        const String& newCmd,
        const std::list<String>& oldCmd
      );

    void addBinding(
        const String& mode,
        const String& keyName,
        const std::list<String>& cmd
      );

    void addFunction(const String& mode, const Function& function);

    void initializeAction(const String& actionName);
    void abortAction();
    void supplyActionArg(const String& actionArg);
    void supplyActionArg(const ActionArgument&);

    void moveMapRelative(sint32 dx, sint32 dy);
    void moveMapRelativeFrac(double dx, double dy);
    void dragRegion(bool start);
    void selectUnits(const String& selection);
    void selectUnitsIn(const sakusen::Rectangle<sint32>&);

    void stop(const std::set<sakusen::UnitId>& units);

    void move(
        const std::set<sakusen::UnitId>& units,
        const ActionTarget& target
      );
    void move(
        const std::set<sakusen::UnitId>& units,
        const sakusen::Position& target
      );
};

}}

#endif // TEDOMARI__UI__UI_H

