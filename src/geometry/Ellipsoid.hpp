#ifndef _ARELLIPSOID_H
#define _ARELLIPSOID_H

#include "Color.hpp"

namespace ar
{

  struct Ellipsoid
  {
    float center[3];

     // Column-major 3x3 matrix with orthogonal axes as columns, ordered by descending size for best results.
    float transform[9];
    float radius;
    Color color;

    Ellipsoid(const double* center, const double* transform, double radius) : color()
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


    Ellipsoid(const double* center, const double* transform, double radius, Color color)
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
