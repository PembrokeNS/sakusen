#include "label.h"

namespace tedomari {
namespace ui {

void Label::paint()
{
    /* Background */
    getRegion()->fill(backColour);
    /* Text */
    getRegion()->drawText(0, 0, text, foreColour);
  }
}

}

