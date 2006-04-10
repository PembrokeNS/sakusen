#include "ui/alertdisplay.h"

using namespace tedomari::ui;

void AlertDisplay::refreshText()
{
  /** \todo Make alerts disappear after a while */
  String text;
  bool first = true;
  for (int i=lastOnDisplay; i != nextSlot; i = (i+1)%9) {
    if (first) {
      first = false;
    } else {
      text += "\n";
    }
    /** \todo Colour, etc. in the text */
    text += alerts[i].getMessage();
  }
  layout->setText(text);
  setDesiredHeight(layout->getHeight());
}

void AlertDisplay::replaceRegion(Region* region)
{
  Control::replaceRegion(region);
  layout->contextChanged();
}

void AlertDisplay::add(const Alert& alert)
{
  alerts[nextSlot++] = alert;
  nextSlot %= 9;
  if (nextSlot == lastOnDisplay) {
    ++lastOnDisplay;
    lastOnDisplay %= 9;
  }
  refreshText();
}

void AlertDisplay::paint()
{
  getRegion()->fill(Colour(0.5, 0.5, 0.5, 0.5));
  getRegion()->drawText(0, 0, layout);
  Control::paint();
}

