#ifndef UPDATEDBALLISTIC_H
#define UPDATEDBALLISTIC_H

namespace sakusen {
namespace client {

/** \brief Represents the client's view of a Ballistic.
 *
 * \note Once a Ballistic has been fired, the client makes no ownership
 * distinction.  Ballistics fired by the enemy are no different from those
 * fired by friendly forces.  (The server does make such a distinction, for its
 * own reasons). */
class LIBSAKUSEN_CLIENT_API UpdatedBallistic : public Bounded {
  public:
    UpdatedBallistic(ClientBallisticId i, const Quadratic& p) :
      id(i),
      path(p)
    {}
  private:
    ClientBallisticId id;
    Quadratic path;
  public:
    ClientBallisticId getId() const { return id; }
    const Quadratic& getPath() const { return path; }

    GameObject getObjectType() const { return gameObject_ballistic; }
    Box<sint32> getBoundingBox() const {
      Position pos = path.evaluate(world->getTimeNow());
      return Box<sint32>(pos, pos);
    }
    boost::tuple<double,double> intersect(const Ray&) const {
      return boost::make_tuple(
          std::numeric_limits<double>::infinity(),
          std::numeric_limits<double>::infinity()
        );
    }
    bool fastContains(const Position& p) const {
      return p == path.evaluate(world->getTimeNow());
    }
    bool contains(const Position& p) const {
      return p == path.evaluate(world->getTimeNow());
    }
};

struct UpdatedBallisticIder {
  ClientBallisticId operator()(const Ref<const UpdatedBallistic>& b) const {
    return b->getId();
  }
};

}}

#endif // UPDATEDBALLISTIC_H

