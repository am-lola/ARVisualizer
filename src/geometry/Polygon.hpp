#ifndef _ARPOLYGON_H
#define _ARPOLYGON_H

#include "Color.hpp"

namespace ar
{

  struct Polygon
  {
    float* points;          // x,y,z coords of each point (length must be 3*numPoints)
    unsigned int numPoints; // number of points in points
    Color color;

    Polygon(double* points, unsigned int count) : color()
    {
      this->points = new float[count*3];
      for (int i = 0; i < count * 3; i++)
      {
        this->points[i] = points[i];
      }

      this->numPoints = count;
    }


    Polygon(double* points, unsigned int count, Color color)
    {
      this->points = new float[count*3];
      for (int i = 0; i < count * 3; i++)
      {
        this->points[i] = points[i];
      }

      this->numPoints = count;
      this->color = color;
    }

  };

} // namespace ar

#endif // _ARPOLYGON_H
