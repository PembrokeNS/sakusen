#ifndef LIBSAKUSEN__FIFO_PRIORITY_QUEUE_H
#define LIBSAKUSEN__FIFO_PRIORITY_QUEUE_H

#include <queue>
#include <list>

#include "gnu_extensions.h"

namespace sakusen {

/** \brief A priority queue which guarantees first-in-first-out behaviour when
 * there is a tie for the highest priority.
 *
 * The price of this functionality is that prioritization can no longer be
 * given in terms of a partial order, but must be given as a value of
 * PriorityType, which must be a totally ordered type.  The three template
 * arguments are:
 * \li ValueType, the type being stored
 * \li PriorityType, the totally ordered type representing priorities.  Because
 *     it's what libsakusen wants, we take smaller values to be higher
 *     priority.
 * \li PrioritizeValueType, a functor taking an argument of const ValueType,
 *     and returning an instance of PriorityType which represents the priority
 *     of that type.
 *
 * Insertion and removal of the top element are O(log(N)).
 *
 * Examination of the top element is O(1).
 *
 * The overhead in insertion and removal can be non-negligible.  Things are
 * better if there are many ValueType instances with each given priority.
 * 
 * This class breaks the libsakusen naming conventions to look more STL-ish
 */
template<
    typename ValueType,
    typename PriorityType,
    typename PrioritizeValueType
  >
class fifo_priority_queue {
  private:
    /* Prioritize is used for the priority_queue of PriorityTypes */
    class Prioritize {
      public:
        inline int operator()( const PriorityType &x, const PriorityType &y ) {
          return x < y;
        }
    };
    /* We store the actual ValueTypes in queues - one for each PriorityType
     * value, all stored together in a hash_map keyed by their priority - and
     * then we use a priority_queue to contain all those values of
     * PriorityType for which a queue exists in the hash_map. */
    typedef typename u_map<
        PriorityType,
        std::queue<ValueType, std::list<ValueType> >
      >::type QueueHashMap;
    std::priority_queue<PriorityType, std::vector<PriorityType>, Prioritize>
      priorityQueue;
    QueueHashMap fifoQueues;
    PrioritizeValueType prioritizer;
  public:
    inline bool empty() const {
      return priorityQueue.empty();
    }
    
    inline void push(const ValueType& val) {
      PriorityType priority = prioritizer(val);
      typename QueueHashMap::iterator queue = fifoQueues.find(priority);
      /* Determine whether we already have a queue for this priority? */
      if (queue == fifoQueues.end()) {
        /* We don't, so we create a new one */
        fifoQueues[priority] = std::queue<ValueType, std::list<ValueType> >();
        priorityQueue.push(priority);
        queue = fifoQueues.find(priority);
        assert(queue != fifoQueues.end());
      }
      queue->second.push(val);
    }

    /* top comes in both const and non-const versions */
    inline ValueType& top() {
      assert(!priorityQueue.empty());
      /* Find the top priority */
      PriorityType topPriority = priorityQueue.top();
      /* Find the queue for that priority */
      typename QueueHashMap::iterator queue = fifoQueues.find(topPriority);
      assert(queue != fifoQueues.end());
      /* Get the element from the front of that queue */
      return queue->second.front();
    }

    inline const ValueType& top() const {
      assert(!priorityQueue.empty());
      /* Find the top priority */
      PriorityType topPriority = priorityQueue.top();
      /* Find the queue for that priority */
      typename QueueHashMap::const_iterator queue =
        fifoQueues.find(topPriority);
      assert(queue != fifoQueues.end());
      /* Get the element from the front of that queue */
      return queue->second.front();
    }

    inline void pop() {
      assert(!priorityQueue.empty());
      /* Find the top priority */
      PriorityType topPriority = priorityQueue.top();
      /* Find the queue for that priority */
      typename QueueHashMap::iterator queue = fifoQueues.find(topPriority);
      assert(queue != fifoQueues.end());
      /* Remove an element from that queue */
      queue->second.pop();
      /* If the queue is emptied then it needs to be removed */
      if (queue->second.empty())
      {
        /* Remove the queue from the hash_map */
        fifoQueues.erase(queue);
        /* Remove the reference to it from the priority_queue */
        priorityQueue.pop();
      }
    }
};

}

#endif // LIBSAKUSEN__FIFO_PRIORITY_QUEUE_H

