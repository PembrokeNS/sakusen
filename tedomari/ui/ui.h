#ifndef TEDOMARI__UI__UI_H
#define TEDOMARI__UI__UI_H

#include "libsakusen-global.h"

#include "ui/key.h"
#include "ui/button.h"

namespace tedomari {
namespace ui {

class UI {
  protected:
    UI() : expectingChars(false), quit(false) {}
    UI(const UI&);
  public:
    virtual ~UI() {}
  private:
    bool expectingChars;
    bool quit;
  protected:
    /** \brief Return true iff we are currently interested in characters input,
     * rather than keys pressed
     *
     * For example, this will be true when currently typing a command or
     * regex, but false when waiting for keypresses which signify commands
     * */
    inline bool isExpectingChars() { return expectingChars; }

    /** \brief Indicate window resize
     *
     * Called by a subclass to indicate that the UI window has been resized,
     * passing the new width and height */
    void resize(uint16 width, uint16 height);

    /** \brief Indicate character input
     *
     * Called by a subclass to indicate the input of a character, passing the
     * unicode character pressed */
    void charInput(uint16 c);

    /** \brief Indicate keydown event
     *
     * Called by a subclass to indicate that a key has been pressed, passing
     * the key in question */
    void keyDown(Key);

    /** \brief Indicate keyup event
     *
     * Called by a subclass to indicate that a key has been released, passing
     * the key in question */
    void keyUp(Key);

    /** \brief Indicate mousedown event
     *
     * Called by a subclass to indicate that a mouse button or wheel has been
     * pressed, passing the button in question and coordinates */
    void mouseDown(Button, uint16 x, uint16 y);

    /** \brief Indicate mouseup event
     *
     * Called by a subclass to indicate that a mouse button has been released,
     * passing the button in question and coordinates */
    void mouseUp(Button, uint16 x, uint16 y);

    /** \brief Indicate quit request */
    inline void quitRequest() { quit = true; }
  public:
    /** \brief Determine whether the user has asked to quit */
    inline bool isQuit() { return quit; }
    /** \brief Set the window title to the given String */
    virtual void setTitle(const String& title) = 0;
    /** \brief Process all user input */
    virtual void update() = 0;
};

}}

#endif // TEDOMARI__UI__UI_H

