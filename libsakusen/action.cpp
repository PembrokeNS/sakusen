#include "action.h"

Action::Action() :
  time(0)
{
}

Action::Action(const Action& copy) :
  time(copy.time)
{
}

Action::~Action()
{
}

