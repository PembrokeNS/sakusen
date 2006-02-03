#ifndef ORIENTATION_H
#define ORIENTATION_H

#include "point.h"
#include "angle.h"
#include "iarchive.h"
#include "oarchive.h"

namespace sakusen {

enum Rotation {
  rotation_anticlockwise
};

enum Reflection {
  reflection_x,
  reflection_y,
  reflection_z
};

/* Class to represent the orientation of a rigid body in three-dimensional
 * space.  The orientation is stored as a matrix which should be applied to the
 * object in its default orientation to yield the current one.
 * Note that since some of our map topologies are non-orientable (mobius and
 * kleinBottle) not all orientations are rotations, they can be reflections
 * too. */

class LIBSAKUSEN_API Orientation {
  public:
    /* a class to facilitate accessing elements of the orientation matrix */
    /* There is some risk of misaccessing of memory if someone should be
     * foolish enough to hold onto an OrientationMatrixRow object until after
     * its corresponding Orientation object is destroyed, but we hope that
     * won't happen, and make all constructors private to make it a little more
     * difficult */
    class OrientationMatrixRow {
      friend class Orientation;
      private:
        OrientationMatrixRow() : row(NULL) {}
        OrientationMatrixRow(const OrientationMatrixRow& copy) :
          row(copy.row) {}
        ~OrientationMatrixRow() {}
        OrientationMatrixRow(const double* r) : row(r) {}
        inline OrientationMatrixRow& operator=(
            const OrientationMatrixRow& copy)
        {
          row = copy.row;
          return *this;
        }

        const double* row;
        
        inline double operator[](const int i) const {
          assert(i>=0);
          assert(i<3);
          return row[i];
        }
    };
    Orientation();
    Orientation(Rotation rotation, Angle amount);
      /* Constructs an orientation corresponding to specified rotation */
    Orientation(Reflection reflection);
      /* Constructs an orientation corresponding to specified reflection */
    ~Orientation() {}
  private:
    Orientation(double matrix[3][3]);
    double matrix[3][3]; /* indexed by rows then columns,
                            matrix acts on the left */
  public:
    inline const OrientationMatrixRow operator[](const int i) const {
      assert(i>=0);
      assert(i<3);
      return OrientationMatrixRow(&(matrix[i][0]));
    }
    Orientation operator*(const Orientation& right) const;
      /* multiplication is non-commutative! */
    bool operator==(const Orientation& right) const;
    inline bool operator!=(const Orientation& right) const {
      return !operator==(right);
    }

    template<typename T>
    Point<T> operator*(const Point<T>& right) const {
      Point<double> result = Point<double>();
      for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
          result[i] += matrix[i][j] * right[j];
        }
      }
      return result.round16();
    }

    void store(OArchive&) const;
    static Orientation load(IArchive&);
};

}

#endif // ORIENTATION_H

