#ifndef _ARPOLYGON_H
#define _ARPOLYGON_H

#include "Color.hpp"

namespace ar
{
  // Struct used to describe simple Polygonal shapes from a list of points
  struct Polygon
  {
    // x,y,z coords of each point (length must be 3*numPoints)
    float* points;

    // number of points in points
    unsigned int numPoints;

    // constant color for the shape
    Color color;

    // Constructor
    // @points 3D coordinates, arranged [x,y,z,x,y,z,...]
    // @count  Total number of 3D points
    Polygon(const double* points, unsigned int count) : color()
    {
      this->points = new float[count*3];
      for (size_t i = 0; i < count * 3; i++)
      {
        this->points[i] = points[i];
      }

      this->numPoints = count;
    }

    // Constructor
    // @points 3D coordinates, arranged [x,y,z,x,y,z,...]
    // @count  Total number of 3D points
    // @color  Color of the mesh
    Polygon(const double* points, unsigned int count, Color color)
    {
      this->points = new float[count*3];
      for (size_t i = 0; i < count * 3; i++)
      {
        this->points[i] = points[i];
      }

      this->numPoints = count;
      this->color = color;
    }

  };

} // namespace ar

#endif // _ARPOLYGON_H
