#ifndef _ARTRANSFORM_H
#define _ARTRANSFORM_H

namespace ar
{
  // Stores translation & rotation parameters to be applied to objects in the visualizer
  struct Transform
  {
    double translation[3];  // x, y, z offset from origin
    double rotation[3][3];  // 3x3 rotation matrix

    Transform()
    {
      // set translation to (0,0,0), and rotation to an identity matrix
      for (int i = 0; i < 3; i++)
      {
        translation[i] = 0.0;
        for (int j = 0; j < 3; j++)
        {
          rotation[i][j] = (i == j ? 1.0 : 0.0);
        }
      }
    }

    Transform(double translation[3])
    {
      // set translation to the input vector, and rotation to an identity matrix
      for (int i = 0; i < 3; i++)
      {
        this->translation[i] = translation[i];
        for (int j = 0; j < 3; j++)
        {
          rotation[i][j] = (i == j ? 1.0 : 0.0);
        }
      }
    }

    Transform(double rotation[3][3])
    {
      // set translation to (0,0,0), and rotation to the input matrix
      for (int i = 0; i < 3; i++)
      {
        translation[i] = 0.0;
        for (int j = 0; j < 3; j++)
        {
          this->rotation[i][j] = rotation[i][j];
        }
      }
    }

    Transform(double translation[3], double rotation[3][3])
    {
      // set translation & rotation to input values
      for (int i = 0; i < 3; i++)
      {
        this->translation[i] = translation[i];
        for (int j = 0; j < 3; j++)
        {
          this->rotation[i][j] = rotation[i][j];
        }
      }
    }

  };

} // namespace ar

#endif // _ARTRANSFORM_H
