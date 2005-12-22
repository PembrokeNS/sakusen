#include "orientation.h"
#include "libsakusen-global.h"

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

Orientation::Orientation(Reflection reflection)
{
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      matrix[i][j] = (i==j);
    }
  }
  
  switch (reflection)
  {
    case reflection_x:
      matrix[0][0] = -1;
      break;
    case reflection_y:
      matrix[1][1] = -1;
      break;
    case reflection_z:
      matrix[2][2] = -1;
      break;
    default:
      Fatal("Unknown Reflection");
  }
}

Orientation::Orientation(double m[3][3])
{
  memcpy(matrix, m, 3*3*sizeof(double));
}

Orientation Orientation::operator*(const Orientation& right) const
{
  double retMatrix[3][3];
  memset(retMatrix, 0, 3*3*sizeof(double));
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      for (int k=0;k<3; k++) {
        retMatrix[i][k] += matrix[i][j] * right[j][k];
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

