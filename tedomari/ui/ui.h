#ifndef TEDOMARI__UI__UI_H
#define TEDOMARI__UI__UI_H

#include "libsakusen-global.h"

#include "stringutils.h"

#include "converter.h"
#include "game/game.h"
#include "ui/key.h"
#include "ui/control.h"
#include "ui/commandentrybox.h"
#include "ui/mode.h"
#include "ui/alert.h"
#include "ui/alertdisplay.h"

namespace tedomari {
namespace ui {

class UI : protected Control {
  private:
    UI();
    UI(const UI&);
  public:
    UI(Region* region, std::ifstream& uiConf);
    virtual ~UI() {}
  private:
    Converter converter; /**< Converts between UTF-8 and native encodings */
    CommandEntryBox* commandEntryBox;
    AlertDisplay* alertDisplay;
    __gnu_cxx::hash_map<String, Mode, sakusen::StringHash> modes;
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
    bool quit;
  protected:
    /** \brief Create and position initial controls
     *
     * Called by a subclass (probably constructor) to create the first controls
     * and place them on the UI.
     */
    void initializeControls(tedomari::game::Game* game);

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
  public:
    /** \brief Determine whether the user has asked to quit */
    inline bool isQuit() const { return quit; }
    /** \brief Set the window title to the given String */
    virtual void setTitle(const String& title) = 0;
    /** \brief Process all user input */
    virtual void update() = 0;
    
    /** \brief Execute the given command
     *
     * \warning Modifies its argument args */
    void executeCommand(const String& cmdName, std::list<String>& args);
    void executeCommand(const String& cmd);
    
    /** \brief Add an alert to the list */
    void alert(const Alert&);
    
    void setCommandEntry(bool on);

    /** \brief Indicate quit request */
    inline void quitRequest() { quit = true; }

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
};

}}

#endif // TEDOMARI__UI__UI_H

