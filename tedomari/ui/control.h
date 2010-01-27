#ifndef TEDOMARI__UI__CONTROL_H
#define TEDOMARI__UI__CONTROL_H

#include <list>

#include "ui/region.h"
#include "ui/dockstyle.h"

namespace tedomari {
namespace ui {

class UI;

class Control {
  friend class UI;
  private:
    Control();
    Control(const Control&);
  public:
    Control(uint16 xoffset, uint16 yoffset, DockStyle ds, Region* r);
    virtual ~Control();
  private:
    uint16 x;
    uint16 y;
    uint16 desiredHeight;
    uint16 desiredWidth;
    DockStyle dockStyle;
    Region* region; /* Owned by this */
    std::list< std::list<Control*> > subControls;
  protected:
    /** \brief Gets the Region occupied by this control */
    inline Region* getRegion() { return region; }
    virtual void replaceRegion(Region* region);
    /** \brief Adds a new layer of subcontrols on top of all existing layers.
     * Subsequently added subcontrols will be added to this new layer.  See
     * Control::alignSubcontrols() for more details on how subcontrols are
     * arranged */
    inline void addLayer() { subControls.push_back(std::list<Control*>()); }
    /** \brief Adds a new subcontrol to the top layer of subcontrols of this
     * Control.  See Control::alignSubcontrols() for more details on how
     * subcontrols are arranged */
    inline void addSubControl(Control* c) { subControls.back().push_back(c); }
    void alignSubControls();
    virtual void paint();
  public:
    /** \name Accessors */
    //@{
    inline DockStyle getDockStyle() { return dockStyle; }
    inline uint16 getX() const { return x; }
    inline uint16 getY() const { return y; }
    inline void setDesiredWidth(uint16 w) { desiredWidth = w; }
    inline void setDesiredHeight(uint16 h) { desiredHeight = h; }
    inline uint16 getWidth() const { return region->getWidth(); }
    inline uint16 getHeight() const { return region->getHeight(); }
    //@}
};

}}

#endif // TEDOMARI__UI__CONTROL_H

