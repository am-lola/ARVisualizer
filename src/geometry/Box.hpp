#ifndef _ARBOX_H
#define _ARBOX_H

#include "Color.hpp"

namespace ar
{

  struct Box
  {
    float center[3];
    float sizeX;
    float sizeY;
    float sizeZ;
    Color color;

    Box(double* center, double sizeX, double sizeY, double sizeZ) : color()
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
      }

      this->sizeX = sizeX;
      this->sizeY = sizeY;
      this->sizeZ = sizeZ;
    }


    Box(double* center, double sizeX, double sizeY, double sizeZ, Color color)
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
      }

      this->sizeX = sizeX;
      this->sizeY = sizeY;
      this->sizeZ = sizeZ;
      this->color = color;
    }

  };

  struct Cube
  {
    float center[3];
    float size;
    Color color;

    Cube(double* center, double size) : color()
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
      }

      this->size = size;
    }


    Cube(double* center, double size, Color color)
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
      }

      this->size = size;
      this->color = color;
    }

  };

} // namespace ar

#endif // _ARBOX_H
