#ifndef _ARSPHERE_H
#define _ARSPHERE_H

#include "color.hpp"

namespace ar
{

  struct Sphere
  {
    float center[3];
    float radius;
    Color color;

    // Creates a sphere. Center is expected to be ordered: x, y, z
    Sphere(double* center, double radius) : color()
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
      }

      this->radius = radius;
    }

    // Creates a sphere. Center is expected to be ordered: x, y, z
    Sphere(double* center, double radius, Color color)
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
      }

      this->radius = radius;
      this->color = color;
    }

    Sphere(double centerX, double centerY, double centerZ, double radius, Color color)
    {
      this->center[0] = centerX;
      this->center[1] = centerY;
      this->center[2] = centerZ;

      this->radius = radius;
      this->color = color;
    }

  };

} // namespace ar

#endif // _ARSPHERE_H
