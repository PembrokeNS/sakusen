#include "orientation.h"
#include "libsakusen-global.h"

using namespace sakusen;

Orientation::Orientation()
{
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      matrix[i][j] = (i==j);
    }
  }
}

Orientation::Orientation(Rotation rotation, Angle amount)
{
  double radians;
  
  switch (rotation) {
    case rotation_anticlockwise:
      radians = double(amount) * M_PI / 180;
      matrix[2][2] = 1;
      matrix[2][0] = matrix[2][1] = matrix[0][2] = matrix[1][2] = 0;
      matrix[0][0] = matrix[1][1] = cos(radians);
      matrix[1][0] = sin(radians);
      matrix[0][1] = -matrix[1][0];
      break;
    default:
      Fatal("Unknown Rotation");
      break;
  }
}

Orientation::Orientation(double m[3][3])
{
  memcpy(matrix, m, 3*3*sizeof(double));
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
}


Orientation Orientation::operator*(const Orientation& right) const
{
  double retMatrix[3][3];
  memset(retMatrix, 0, 3*3*sizeof(double));
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      for (int k=0;k<3; k++) {
        retMatrix[i][k] += matrix[i][j] * right(j, k);
      }
    }
  }
  return Orientation(retMatrix);
}

bool Orientation::operator==(const Orientation& right) const
{
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      /* Beware: use of != operator between floating point types may lead to
       * precision problems */
      if (matrix[i][j] != right.matrix[i][j]) {
        return false;
      }
    }
  }
  return true;
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

