#ifndef _ARLINE_H
#define _ARLINE_H

#include "Color.hpp"
#include "../CircularBuffer.hpp"

namespace ar
{
  // A line going through a set points in order
  struct LinePath
  {
    // x,y,z coords of each point (size must be 3*numPoints)
    std::vector<float> points;
    float thickness;
    Color color;

    LinePath(float thickness = 0.004f, Color color = Color())
      : thickness(thickness), color(color)
    {
    }

    LinePath(const double* points, size_t count, float thickness = 0.004f, Color color = Color())
      : thickness(thickness), color(color)
    {
      this->points.assign(points, points + count * 3);
    }

    void addPoint(const double* point)
    {
      points.insert(points.end(), point, point + 3);
    }
  };

  // An implementation of a <LinePath> using an underlying circular buffer. Once it reaches full size, adding a new point will remove the oldest point.
  // This is useful for visualizing trajectories.
  struct BufferedLinePath : public LinePath
  {
  private:

    CircularBuffer<float> buffer;

  public:

    BufferedLinePath(size_t size, float thickness = 0.004f, Color color = Color())
      : LinePath(thickness, color), buffer(3 * size)
    {
    }

    BufferedLinePath(size_t size, const double* points, size_t count, float thickness = 0.004f, Color color = Color())
      : LinePath(thickness, color), buffer(3 * size)
    {
      for (size_t i = 0; i < count * 3; i++)
        buffer.push_back(points[i]);

      buffer.toVector(LinePath::points);
    }

    void addPoint(const double* point)
    {
      for (int i = 0; i < 3; i++)
        buffer.push_back(point[i]);

      buffer.toVector(LinePath::points);
    }

    void resize(size_t newSize)
    {
      buffer.resize(3 * newSize);
    }
  };

  // A line segment between two points
  struct LineSegment : public LinePath
  {
    LineSegment(const double* center1, const double* center2, float thickness = 0.004f, Color color = Color()) : LinePath(thickness, color)
    {
      points.assign(center1, center1 + 3);
      points.insert(points.end(), center2, center2 + 3);

      this->thickness = thickness;
    }
  };
} // namespace ar

#endif // _ARLINE_H
