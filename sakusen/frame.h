#ifndef LIBSAKUSEN__FRAME_H
#define LIBSAKUSEN__FRAME_H

#include <sakusen/point.h>
#include <sakusen/orientation.h>
#include <sakusen/box.h>

namespace sakusen {

/** \brief Represents a frame of reference, i.e. a position and orientation.
 *
 * The position represents the position of the origin in this frame, and the
 * orientation is the orientation of the axes.
 *
 * \todo We might want to change the internal representation to a matrix a
 * la computer graphics. */
class LIBSAKUSEN_API Frame {
  public:
    Frame() {}
    Frame(const Position& p, const Orientation& o) :
      position(p),
      orientation(o)
    {}
  private:
    Position position;
    Orientation orientation;
  public:
    Position& getPosition() { return position; }
    const Position& getPosition() const { return position; }
    Orientation& getOrientation() { return orientation; }
    const Orientation& getOrientation() const { return orientation; }

    Frame& operator+=(const Position& p) {
      position += p;
      return *this;
    }

    Frame operator+(const Position& p) {
      return Frame(*this) += p;
    }

    Box<sint32> getBoundingBox(const Point<uint32>& size) const;

    /** \brief Converts an position in local to absolute global coordinates.
     *
     * Local coordinates are those of the Frame.
     *
     * Absolute global coordinates are the usual coordinate system.
     */
    inline Point<sint32> localToGlobal(const Point<sint32>& p) const {
      return getOrientation()*p+getPosition();
    }

    /** \brief Converts an position in local to relative global coordinates.
     *
     * Local coordinates are those of the Frame.
     *
     * Relative global coordinates are aligned with the usual coordinates, but
     * with their origin at the origin of the Frame.
     */
    inline Point<sint32> localToGlobalRelative(const Point<sint32>& p) const {
      return getOrientation()*p;
    }

    /** \brief Converts an position in absolute global to local coordinates.
     *
     * Local coordinates are those of the Frame.
     *
     * Absolute global coordinates are the usual coordinate system.
     */
    inline Point<sint32> globalToLocal(const Point<sint32>& p) const {
      return getOrientation().inverseMul(p-getPosition());
    }

    /** \brief Converts an position in relative global to local coordinates.
     *
     * Local coordinates are those of the Frame.
     *
     * Relative global coordinates are aligned with the usual coordinates, but
     * with their origin at the origin of the Frame.
     */
    inline Point<sint32> globalToLocalRelative(const Point<sint32>& p) const {
      return getOrientation().inverseMul(p);
    }

    void store(OArchive&) const;
    static Frame load(IArchive&);
};

}

#endif // LIBSAKUSEN__FRAME_H

