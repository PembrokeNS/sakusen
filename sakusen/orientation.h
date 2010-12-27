#ifndef LIBSAKUSEN__ORIENTATION_H
#define LIBSAKUSEN__ORIENTATION_H

#include <sakusen/global.h>

#include <sakusen/point.h>
#include <sakusen/angle.h>
#include <sakusen/iarchive.h>
#include <sakusen/oarchive.h>

namespace sakusen {

/** \brief Enumeration of directions of rotation */
enum Rotation {
  /** Anticlockwise around a vertical axis as viewed from above.  This is a
   * positive z rotation because we use a right-handed coordinate system. */
  rotation_anticlockwise
};

/** \brief Class to represent the orientation of a rigid body in
 * three-dimensional space.
 *
 * The orientation is stored as a matrix which should be applied to the
 * object in its default orientation to yield the current one.
 * Note that since some of our map topologies are non-orientable (mobius and
 * kleinBottle) not all orientations are rotations, they can be reflections
 * too. */
class LIBSAKUSEN_API Orientation {
  public:
    Orientation();
    Orientation(Rotation rotation, Angle amount);
    Orientation(const AngularVelocity&);
    ~Orientation() {}
  private:
    Orientation(const double[3][3]);
    Orientation(const Point<double>[3]);
    Orientation(
        double, double, double, double, double, double, double, double, double
      );
    double matrix[3][3]; /* indexed by rows then columns,
                            matrix acts on the left */
  public:
    inline double operator()(unsigned int i, unsigned int j) const {
      assert(i<3);
      assert(j<3);
      return matrix[i][j];
    }
    Orientation operator*(const Orientation& right) const;
    bool operator==(const Orientation& right) const;
    inline bool operator!=(const Orientation& right) const {
      return !operator==(right);
    }

    /** \brief Multiply this matrix by given vector.
     *
     * T is asumed to be integral.  Result is rounded to nearest integer.
     */
    template<typename T>
    Point<T> operator*(const Point<T>& right) const {
      Point<double> result = Point<double>();
      for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
          result[i] += matrix[i][j] * right[j];
        }
      }
      return Point<T>(result.round());
    }

#ifdef NDEBUG
    void sanityCheck() const {}
#else
    void sanityCheck() const;
#endif

    /** \brief Compute the trace of this matrix */
    double trace() const { return matrix[0][0] + matrix[1][1] + matrix[2][2]; }

    /** \brief Return the inverse of this matrix.
     *
     * It is possible to do this operation quickly because we know the matrix
     * is orthogonal and thus its inverse is its transpose.
     */
    Orientation inverse() const {
      return Orientation(
          matrix[0][0], matrix[1][0], matrix[2][0],
          matrix[0][1], matrix[1][1], matrix[2][1],
          matrix[0][2], matrix[1][2], matrix[2][2]
        );
    }

    /** \brief Multiply the inverse of this matrix by the given vector
     *
     * It is possible to do this operation quickly because we know the matrix
     * is orthogonal and thus its inverse is its transpose. */
    template<typename T>
    Point<T> inverseMul(const Point<T>& right) const {
      Point<double> result = Point<double>();
      for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
          result[i] += matrix[j][i] * right[j];
        }
      }
      return Point<T>(result.round());
    }

    /** \brief Find angular velocity generating this Orientation.
     *
     * Computes and return the angular velocity which would turn the
     * identity into this Orientation in one tick. */
    AngularVelocity getGeneratingAngularVelocity();

    /** \name Some particular orientations.
     *
     * We provide the identity and reflections in the plane perpendicular to
     * each axis as static values. */
    //@{
    static const Orientation identity;
    static const Orientation reflectionX;
    static const Orientation reflectionY;
    static const Orientation reflectionZ;
    //@}

    void store(OArchive&) const;
    static Orientation load(IArchive&);
};

}

#endif // LIBSAKUSEN__ORIENTATION_H

