#ifndef FUSE_H
#define FUSE_H

#include "libsakusen-global.h"
#include "fifo_priority_queue.h"

namespace sakusen {
namespace server {

typedef uint32 FuseToken;

class Fuse {
  protected:
    /* Fuse is an abstract class, so all constructors are protected. */
    Fuse();
    Fuse(const Fuse&);
  public:
    virtual ~Fuse();
  public:
    /** \brief Called when the time for the Fuse has arrived
     *
     * Returns true iff this Fuse should now be deleted.
     */
    virtual bool expire(FuseToken) const = 0;
};

struct FuseEntry {
  FuseEntry() : fuse(NULL), time(0), token(0) {}
  FuseEntry(Fuse* f, Time t, FuseToken ft) : fuse(f), time(t), token(ft) {}
  Fuse* fuse;
  Time time;
  FuseToken token;
};

class PrioritizeFuseEntries {
  public:
    inline uint32 operator()(const FuseEntry& fuseEntry) {
      return fuseEntry.time;
    }
};

typedef fifo_priority_queue<FuseEntry, uint32, PrioritizeFuseEntries> FuseQueue;

}}

#endif

