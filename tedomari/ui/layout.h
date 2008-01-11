#ifndef UI__LAYOUT_H
#define UI__LAYOUT_H

#include "tedomari-global.h"

#include "ui/colour.h"

namespace tedomari {
namespace ui {

/** \brief Abstract class representing some text intended for rendering to the
 * GUI
 *
 * This class has methods for setting the text to be rendered, and it colour,
 * and can return the height which this text will take up when rendered.
 * 
 * It is expected that this class have subclasses implementing its methods for
 * each particular text rendering method. */
class Layout {
  protected:
    /** \brief Default constructor
     *
     * Initializes Layout with colour white. */
    Layout() : colour(Colour::white) {}
    /** \brief Copy constructor */
    Layout(const Layout& copy) : colour(copy.colour) {}
  private:
    Colour colour;
  public:
    virtual ~Layout() {}
   
    /** \name Accessors */
    //@{
    inline const Colour& getColour() const { return colour; }
    inline void setColour(const Colour& c) { colour = c; }
    virtual void setText(const String&) = 0;
    virtual uint16 getHeight() const = 0;
    //@}

    /** \brief Indicate that underlying surface has changed
     *
     * This method can be called on a Layout to inform it that the underlying
     * surface used by the GUI has changed, so that it can take appropriate
     * action (probably reinitializing itself in some way). */
    virtual void contextChanged() = 0;
};

}}

#endif // UI__LAYOUT_H

