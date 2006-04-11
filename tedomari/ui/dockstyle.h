#ifndef UI__DOCKSTYLE_H
#define UI__DOCKSTYLE_H

namespace tedomari {
namespace ui {

/** \brief Enumeration of docking styles that can be assumed by a Control.
 *
 * A DockStyle indicates how a control would prefer to be aligned within its
 * parent control. */
enum DockStyle {
  /** Place the control across the full width of its parent, at the top */
  dockStyle_top,
  /** Place the control across the full width of its parent, at the bottom */
  dockStyle_bottom,
  /** Place the control across the full height of its parent, at the left */
  dockStyle_left,
  /** Place the control across the full height of its parent, at the right */
  dockStyle_right,
  /** Cover the entire parent control */
  dockStyle_fill
};

}}

#endif // UI__DOCKSTYLE_H

