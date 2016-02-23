#ifndef _ARQUAD_H
#define _ARQUAD_H

#include "color.hpp"

namespace ar
{

  struct Quad
  {
    float center[3];
    float normal[3];
    float width;
    float height;
    Color color;

    // Creates a quad. Center & normal are expected to be ordered: x, y, z
    Quad(double* center, double* normal) : width(0.0f), height(0.0f), color()
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
    Quad(double* center, double* normal, double width, double height) : color()
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
    Quad(double* center, double* normal, double width, double height, Color color)
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

  };

} // namespace ar

#endif // _ARQUAD_H
