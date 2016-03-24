#ifndef _ARQUAD_H
#define _ARQUAD_H

#include "Color.hpp"

namespace ar
{
  // Describes a rectangle in 3D space
  struct Quad
  {
    float center[3];
    float normal[3];
    float width;
    float height;
    Color color;

    // Creates a quad. Center & normal are expected to be ordered: x, y, z
    Quad(const double* center, const double* normal) : width(0.0f), height(0.0f), color()
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
        this->normal[i] = normal[i];
      }
    }

    // Creates a quad. Center & normal are expected to be ordered: x, y, z
    Quad(const double* center, const double* normal, double width, double height) : color()
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
        this->normal[i] = normal[i];
      }

      this->width = width;
      this->height = height;
    }

    // Creates a quad. Center & normal are expected to be ordered: x, y, z
    Quad(const double* center, const double* normal, double width, double height, Color color)
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
        this->normal[i] = normal[i];
      }

      this->width = width;
      this->height = height;
      this->color = color;
    }

    Quad(double x, double y, double z, double* normal, double width, double height, Color color)
    {
      this->center[0] = x;
      this->center[1] = y;
      this->center[2] = z;

      for (int i = 0; i < 3; i++)
      {
        this->normal[i] = normal[i];
      }

      this->width = width;
      this->height = height;
      this->color = color;
    }

  };

} // namespace ar

#endif // _ARQUAD_H
