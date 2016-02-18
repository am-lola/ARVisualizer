#ifndef _ARCAPSULE_H
#define _ARCAPSULE_H

#include "color.hpp"

namespace ar
{

  struct Capsule
  {
    float center1[3];
    float center2[3];
    float radius;
    Color color;

    // Creates a capsule. Centers are expected to be ordered: x, y, z
    Capsule(float* center1, float* center2, float radius) : color()
    {
      for (int i = 0; i < 3; i++)
      {
        this->center1[i] = center1[i];
        this->center2[i] = center2[i];
      }

      this->radius = radius;
    }

    // Creates a capsule. Centers are expected to be ordered: x, y, z
    Capsule(float* center1, float* center2, float radius, Color color)
    {
      for (int i = 0; i < 3; i++)
      {
        this->center1[i] = center1[i];
        this->center2[i] = center2[i];
      }

      this->radius = radius;
      this->color = color;
    }

  };

} // namespace ar

#endif // _ARCAPSULE_H
