#include <sakusen/orientation.h>

using namespace sakusen;

/** \brief Constructs the identity Orientation */
Orientation::Orientation()
{
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      matrix[i][j] = (i==j);
    }
  }

  sanityCheck();
}

/** \brief Constructs an Orientation representing a rotation.
 *
 * The rotation constructed is that by \p amount centidegrees
 * around the axis specified by \p rotation.
 */
Orientation::Orientation(Rotation rotation, Angle amount)
{
  double radians;

  switch (rotation) {
    case rotation_anticlockwise:
      radians = double(amount) * M_PI / 18000;
      matrix[2][2] = 1;
      matrix[2][0] = matrix[2][1] = matrix[0][2] = matrix[1][2] = 0;
      matrix[0][0] = matrix[1][1] = cos(radians);
      matrix[1][0] = sin(radians);
      matrix[0][1] = -matrix[1][0];
      break;
    default:
      SAKUSEN_FATAL("Unknown Rotation");
      break;
  }

  sanityCheck();
}

/** \brief Constructs an Orientation representing a rotation.
 *
 * The rotation constructed is around the axis specified by \p axis, and is by
 * a number of centidegrees equal to the length of \p axis.
 */
Orientation::Orientation(const AngularVelocity& axis)
{
  const double theta = axis.length() * M_PI / 18000;
  Point<double> normalised;
  if (theta != 0) {
    normalised = Point<double>(axis);
    normalised.normalise();
  }

  for (int i=0; i<3; ++i) {
    for (int j=0; j<3; ++j) {
      matrix[i][j] = normalised[i]*normalised[j]*(1-cos(theta));
    }
    matrix[i][i] += cos(theta);
  }

  matrix[0][2] += normalised[1] * sin(theta);
  matrix[1][0] += normalised[2] * sin(theta);
  matrix[2][1] += normalised[0] * sin(theta);

  matrix[2][0] -= normalised[1] * sin(theta);
  matrix[0][1] -= normalised[2] * sin(theta);
  matrix[1][2] -= normalised[0] * sin(theta);

  sanityCheck();
}

Orientation::Orientation(const double m[3][3])
{
  memcpy(matrix, m, 3*3*sizeof(double));

  sanityCheck();
}

Orientation::Orientation(const Point<double> m[3])
{
  matrix[0][0] = m[0][0]; matrix[0][1] = m[0][1]; matrix[0][2] = m[0][2];
  matrix[1][0] = m[1][0]; matrix[1][1] = m[1][1]; matrix[1][2] = m[1][2];
  matrix[2][0] = m[2][0]; matrix[2][1] = m[2][1]; matrix[2][2] = m[2][2];

  sanityCheck();
}

Orientation::Orientation(
    double e00, double e01, double e02,
    double e10, double e11, double e12,
    double e20, double e21, double e22
  )
{
  matrix[0][0] = e00; matrix[0][1] = e01; matrix[0][2] = e02;
  matrix[1][0] = e10; matrix[1][1] = e11; matrix[1][2] = e12;
  matrix[2][0] = e20; matrix[2][1] = e21; matrix[2][2] = e22;

  sanityCheck();
}

/** \brief Performs composition of Orientations
 *
 * \warning Multiplication is non-commutative! */
Orientation Orientation::operator*(const Orientation& right) const
{
  Point<double> retMatrix[3];
  memset(retMatrix, 0, 3*3*sizeof(double));
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      for (int k=0;k<3; k++) {
        retMatrix[i][k] += matrix[i][j] * right(j, k);
      }
    }
  }
  /** \bug Because of numerical errors, this operator can produce a matrix
   * which isn't orthogonal.  The following is a hacky fix to force it to be
   * orthogonal.  Someone who understands numerical analysis better should come
   * and fix this. */

  /* Normalise row 1 */
  retMatrix[1].normalise();

  /* Set row 2 to be orthonormal to the other two */
  {
    Point<double> row2 = retMatrix[0].crossProduct(retMatrix[1]);
    if (row2.innerProduct(retMatrix[2]) > 0) {
      retMatrix[2] = row2;
    } else {
      retMatrix[2] = -row2;
    }
  }
  retMatrix[2].normalise();

  /* Set row 0 to be orthonormal to the other two */
  {
    Point<double> row0 = retMatrix[1].crossProduct(retMatrix[2]);
    if (row0.innerProduct(retMatrix[0]) > 0) {
      retMatrix[0] = row0;
    } else {
      retMatrix[0] = -row0;
    }
  }
  retMatrix[0].normalise();

  return Orientation(retMatrix);
}

bool Orientation::operator==(const Orientation& right) const
{
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      /** \bug Use of != operator between floating point types may lead to
       * precision problems */
      if (matrix[i][j] != right.matrix[i][j]) {
        return false;
      }
    }
  }
  return true;
}

#ifndef NDEBUG
/** \brief Conducts some sanity checks on the Orientation.
 *
 * This verifies some properties of the Orientation, e.g. that all of its
 * entries are in the range [-1,1].
 *
 * When NDEBUG is defined, this method does nothing and is inlined, so should
 * cost nothing to call.
 */
void Orientation::sanityCheck() const
{
  for (int i=0; i<3; ++i) {
    for (int j=0; j<3; ++j) {
      assert(!std::isnan(matrix[i][j]));
      assert(matrix[i][j] >= -1);
      assert(matrix[i][j] <= 1);
    }
  }
}
#endif

AngularVelocity Orientation::getGeneratingAngularVelocity()
{
  const double cosTheta = (trace() - 1) / 2;
  assert(cosTheta <= 1);
  assert(cosTheta >= -1);
  const double theta = acos(cosTheta);
  if (theta == 0) {
    return AngularVelocity();
  } else if (sin(theta) == 0) {
    /** \bug This isn't always the right answer in this case; need to think
     * more */
    return AngularVelocity(0, 0, 18000);
  }
  /* Construct a vector pointing in the right direction */
  Point<double> angularVelocity(
      matrix[1][2] - matrix[2][1],
      matrix[2][0] - matrix[0][2],
      matrix[0][1] - matrix[1][0]
    );
  /* Normalize it */
  angularVelocity *= -0.5/sin(theta);
  /* Now we scale it to a value appropriate for working with degrees, rather
   * than radians (because we use integer arithmetic mostly, radians are much
   * too big) */
  angularVelocity *= theta*18000/M_PI;
  return AngularVelocity(angularVelocity);
}

const Orientation LIBSAKUSEN_API Orientation::identity = Orientation();
const Orientation Orientation::reflectionX =
  Orientation(-1, 0, 0, 0,  1, 0, 0, 0,  1);
const Orientation Orientation::reflectionY =
  Orientation( 1, 0, 0, 0, -1, 0, 0, 0,  1);
const Orientation Orientation::reflectionZ =
  Orientation( 1, 0, 0, 0,  1, 0, 0, 0, -1);

void Orientation::store(OArchive& archive) const
{
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      archive << matrix[i][j];
    }
  }
}

Orientation Orientation::load(IArchive& archive)
{
  double matrix[3][3];

  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      archive >> matrix[i][j];
    }
  }

  return Orientation(matrix);
}

