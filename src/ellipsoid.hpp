#ifndef _ARELLIPSOID_H
#define _ARELLIPSOID_H

#include "color.hpp"

namespace ar
{

  struct Ellipsoid
  {
    float center[3];
    float transform[9]; // Column-major 3x3 matrix with orthogonal axes as columns, ordered by descending size for best results.
    float radius;
    Color color;

    Ellipsoid(float* center, float* transform, float radius) : color()
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
      }
      for (int i = 0; i < 9; i++)
      {
        this->transform[i] = transform[i];
      }

      this->radius = radius;
    }


    Ellipsoid(float* center, float* transform, float radius, Color color)
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
      }
      for (int i = 0; i < 9; i++)
      {
        this->transform[i] = transform[i];
      }

      this->radius = radius;
      this->color = color;
    }

  };

} // namespace ar

#endif // _ARELLIPSOID_H
