#ifndef _ARCAPSULE_H
#define _ARCAPSULE_H

#include "Color.hpp"

namespace ar
{

  struct Capsule
  {
    float center1[3];
    float center2[3];
    float radius;
    Color color;

    // Creates a capsule. Centers are expected to be ordered: x, y, z
    Capsule(double* center1, double* center2, double radius) : color()
    {
      for (int i = 0; i < 3; i++)
      {
        this->center1[i] = center1[i];
        this->center2[i] = center2[i];
      }

      this->radius = radius;
    }

    // Creates a capsule. Centers are expected to be ordered: x, y, z
    Capsule(double* center1, double* center2, double radius, Color color)
    {
      for (int i = 0; i < 3; i++)
      {
        this->center1[i] = center1[i];
        this->center2[i] = center2[i];
      }

      this->radius = radius;
      this->color = color;
    }

    // Creates a capsule.
    Capsule(double center1_x, double center1_y, double center1_z, double center2_x, double center2_y, double center2_z, double radius, Color color)
    {
      this->center1[0] = center1_x;
      this->center1[1] = center1_y;
      this->center1[2] = center1_z;

      this->center2[0] = center2_x;
      this->center2[1] = center2_y;
      this->center2[2] = center2_z;

      this->radius = radius;
      this->color = color;
    }

  };

} // namespace ar

#endif // _ARCAPSULE_H
