#ifndef _ARTRIANGLE_H
#define _ARTRIANGLE_H

#include "Color.hpp"

namespace ar
{

  struct Triangle
  {
    float p1[3];
    float p2[3];
    float p3[3];
    Color color;

    // Creates a triangle. Expects vertex_positions in the order: x1, y1, z1, x2, y2, z2, ...
    Triangle(const double* vertex_positions) : color()
    {
      for (int i = 0; i < 3; i++)
      {
        p1[i] = vertex_positions[i];
        p2[i] = vertex_positions[i+3];
        p3[i] = vertex_positions[i+6];
      }
    }

    // Creates a triangle. Points are expected to be ordered: x, y, z
    Triangle(double p1[3], double p2[3], double p3[3]) : color()
    {
      for (int i = 0; i < 3; i++)
      {
        this->p1[i] = p1[i];
        this->p2[i] = p2[i];
        this->p3[i] = p3[i];
      }
    }

    // Creates a triangle. Expects vertex_positions in the order: x1, y1, z1, x2, y2, z2, ...
    Triangle(const double* vertex_positions, Color color)
    {
      this->color = color;

      for (int i = 0; i < 3; i++)
      {
        p1[i] = vertex_positions[i];
        p2[i] = vertex_positions[i+3];
        p3[i] = vertex_positions[i+6];
      }
    }

    // Creates a triangle. Points are expected to be ordered: x, y, z
    Triangle(double p1[3], double p2[3], double p3[3], Color color)
    {
      this->color = color;

      for (int i = 0; i < 3; i++)
      {
        this->p1[i] = p1[i];
        this->p2[i] = p2[i];
        this->p3[i] = p3[i];
      }
    }
  };

} // namespace ar

#endif // _ARTRIANGLE_H
