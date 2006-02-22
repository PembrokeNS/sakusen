#include "ui/alertdisplay.h"

using namespace tedomari::ui;

/* TODO: make alerts disappear after a while */

void AlertDisplay::refreshText()
{
  String text;
  bool first = true;
  for (int i=lastOnDisplay; i != nextSlot; i = (i+1)%9) {
    if (first) {
      first = false;
    } else {
      text += "\n";
    }
    /* TODO: colour, etc. in the text */
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
  getRegion()->flood(Colour(0.5, 0.5, 0.5, 0.5));
  getRegion()->drawText(0, 0, layout);
  Control::paint();
}

