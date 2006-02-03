#ifndef ACTION_H
#define ACTION_H

#include "libsakusen-global.h"
#include "fifo_priority_queue.h"

namespace sakusen {

class PrioritizeActions;

class Action {
  protected:
    /* Action is an abstract class, and as such it is critical that all its
     * constructors be protected - making them public could lead to Unmitigated
     * Disaster, so don't do it. */
    Action();
    Action(const Action&);
  public:
    virtual ~Action();
    virtual Action* newCopy(void) const = 0;
  protected:
    Time time; /* Time that this action will occur */
  public:
    /* accessors */
    inline const Time& getTime() const { return time; }
    
    virtual void act() const = 0;
};

class PrioritizeActions {
  public:
    inline uint32 operator()(const Action* action) {
      return action->getTime();
    }
};

typedef fifo_priority_queue<Action*, uint32, PrioritizeActions> ActionQueue;

}

#endif

