#ifndef _ARBOX_H
#define _ARBOX_H

#include "color.hpp"

namespace ar
{

  struct Box
  {
    float center[3];
    float sizeX;
    float sizeY;
    float sizeZ;
    Color color;

    Box(float* center, float sizeX, float sizeY, float sizeZ) : color()
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
      }

      this->sizeX = sizeX;
      this->sizeY = sizeY;
      this->sizeZ = sizeZ;
    }


    Box(float* center, float sizeX, float sizeY, float sizeZ, Color color)
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

    Cube(float* center, float size) : color()
    {
      for (int i = 0; i < 3; i++)
      {
        this->center[i] = center[i];
      }

      this->size = size;
    }


    Cube(float* center, float size, Color color)
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
