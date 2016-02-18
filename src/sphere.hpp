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

    // Creates a quad. Center & normal are expected to be ordered: x, y, z
    Sphere(float* center, float radius) : color()
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
      }

      this->radius = radius;
    }

    // Creates a quad. Center & normal are expected to be ordered: x, y, z
    Sphere(float* center, float radius, Color color)
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
      }

      this->radius = radius;
      this->color = color;
    }

  };

} // namespace ar

#endif // _ARSPHERE_H
