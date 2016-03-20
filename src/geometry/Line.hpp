#ifndef _ARLINE_H
#define _ARLINE_H

#include "Color.hpp"
#include <vector>

namespace ar
{
  // A line going through a set points in order
  struct LinePath
  {
    // x,y,z coords of each point (size must be 3*numPoints)
    std::vector<float> points;
    float thickness;
    Color color;

    LinePath(double* points, size_t count, float thickness = 0.004f, Color color = Color())
      : thickness(thickness), color(color)
    {
      this->points.assign(points, points + count * 3);
    }

    LinePath(float thickness, Color color)
      : thickness(thickness), color(color)
    {

    }
  };

  // A line segment between two points
  struct LineSegment : public LinePath
  {
    LineSegment(double* center1, double* center2, float thickness = 0.004f, Color color = Color()) : LinePath(thickness, color)
    {
      points.assign(center1, center1 + 3);
      points.insert(points.end(), center2, center2 + 3);

      this->thickness = thickness;
    }
  };
} // namespace ar

#endif // _ARLINE_H
