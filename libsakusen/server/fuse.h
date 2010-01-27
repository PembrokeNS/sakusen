#ifndef LIBSAKUSEN_SERVER__FUSE_H
#define LIBSAKUSEN_SERVER__FUSE_H

#include "libsakusen-global.h"
#include "fifo_priority_queue.h"

namespace sakusen {
namespace server {

/** \brief Token uniquely identifying a Fuse.
 *
 * \see Fuse */
typedef uint32 FuseToken;

/** \brief Represents an event which will occur at some point in the future.
 *
 * When a module wishes to postpone some action rather than do it at once, it
 * should add a Fuse to world, specifying the time that the Fuse expires.  When
 * that exiry occurs, Fuse::expire will be called and the appropriate action
 * can be performed.
 *
 * When the Fuse is added to the world, a FuseToken will be returned, and then
 * this same token will be passed to Fuse::expire.  This allows the same Fuse
 * to be registered multiple times and perform different actions according to
 * the token, if desired.
 */
class Fuse {
  public:
    typedef boost::shared_ptr<Fuse> Ptr;
    typedef boost::shared_ptr<const Fuse> ConstPtr;
  protected:
    /* Fuse is an abstract class, so all constructors are protected. */
    Fuse() {}
    Fuse(const Fuse&) {}
  public:
    virtual ~Fuse() {}
  public:
    /** \brief Called when the time for the Fuse has arrived.
     */
    virtual void expire(FuseToken) = 0;
};

/** \brief Combines a FuseToken with an expiry time for ease of inserting into
 * a FuseQueue
 */
struct FuseEntry {
  FuseEntry() : time(0), token(0) {}
  FuseEntry(Time t, FuseToken ft) : time(t), token(ft) {}
  Time time;
  FuseToken token;
};

/** \brief Function object fetching the time of a FuseEntry.
 *
 * This is intended for use in a fifo_priority_queue.
 */
class PrioritizeFuseEntries {
  public:
    inline Time operator()(const FuseEntry& fuseEntry) {
      /* negate time to get fuses in correct order */
      return ~fuseEntry.time;
    }
};

/** \brief Typedef for container that efficiently stores FuseEntries.
 *
 * This typedef provides a container which allows insertion of FuseEntries and
 * removal of the next applicable FuseEntry in O(log(n)) time.  Moreover, it is
 * guaranteed to return the FuseEntries in the same order on any platform, even
 * when two have equal times.
 */
typedef fifo_priority_queue<FuseEntry, Time, PrioritizeFuseEntries> FuseQueue;

}}

#endif // LIBSAKUSEN_SERVER__FUSE_H

