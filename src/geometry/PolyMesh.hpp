#ifndef _ARPOLYMESH_H
#define _ARPOLYMESH_H

#include "Color.hpp"

namespace ar
{
  // Raw mesh format to render arbitrary geometry
  struct PolyMesh
  {
    // x,y,z coords of each point (length must be 3*numPoints)
    float* points;

    // number of points in points
    unsigned int numPoints;

    // [Optional] If filled, triangles will be generated as (points[indices[0]], points[indices[1]], points[indices[2]], ...)
    unsigned int* indices;

    // number of indices in indices
    unsigned int numIndices;

    // [Optional] If filled, must contain one normal per point. If not set, normals will be automatically generated.
    float* normals;

    // Constant color for the whole mesh
    Color color;

    PolyMesh(const double* points, unsigned int count) : numPoints(count), indices(0), numIndices(0), normals(0)
    {
      this->points = new float[count*3];
      for (size_t i = 0; i < count * 3; i++)
      {
        this->points[i] = points[i];
      }
    }

    PolyMesh(const double* points, unsigned int count, Color color) : numPoints(count), indices(0), numIndices(0), normals(0), color(color)
    {
      this->points = new float[count*3];
      for (size_t i = 0; i < count * 3; i++)
      {
        this->points[i] = points[i];
      }
    }

    PolyMesh(const double* points, unsigned int count, const double* normals, Color color) : numPoints(count), indices(0), numIndices(0), color(color)
    {
      this->points = new float[count*3];
      this->normals = new float[count*3];
      for (size_t i = 0; i < count * 3; i++)
      {
        this->points[i] = points[i];
        this->normals[i] = normals[i];
      }
    }

    PolyMesh(const double* points, unsigned int pointsCount, const unsigned int* indices, unsigned int indexCount) : numPoints(pointsCount), numIndices(indexCount), normals(0)
    {
      this->points = new float[pointsCount*3];
      for (size_t i = 0; i < pointsCount * 3; i++)
      {
        this->points[i] = points[i];
      }

      this->indices = new unsigned int[indexCount];
      for (size_t i = 0; i < indexCount; i++)
      {
        this->indices[i] = indices[i];
      }
    }

    PolyMesh(const double* points, unsigned int pointsCount, const unsigned int* indices, unsigned int indexCount, const double* normals) : numPoints(pointsCount), numIndices(indexCount)
    {
      this->points = new float[pointsCount*3];
      this->normals = new float[pointsCount*3];
      for (size_t i = 0; i < pointsCount * 3; i++)
      {
        this->points[i] = points[i];
        this->normals[i] = normals[i];
      }

      this->indices = new unsigned int[indexCount];
      for (size_t i = 0; i < indexCount; i++)
      {
        this->indices[i] = indices[i];
      }
    }

    PolyMesh(const double* points, unsigned int pointsCount, const unsigned int* indices, unsigned int indexCount, Color color) : numPoints(pointsCount), numIndices(indexCount), normals(0), color(color)
    {
      this->points = new float[pointsCount*3];
      for (size_t i = 0; i < pointsCount * 3; i++)
      {
        this->points[i] = points[i];
      }

      this->indices = new unsigned int[indexCount];
      for (size_t i = 0; i < indexCount; i++)
      {
        this->indices[i] = indices[i];
      }
    }

    PolyMesh(const double* points, unsigned int pointsCount, const unsigned int* indices, unsigned int indexCount, const double* normals, Color color) : numPoints(pointsCount), numIndices(indexCount), color(color)
    {
      this->points = new float[pointsCount*3];
      this->normals = new float[pointsCount*3];
      for (size_t i = 0; i < pointsCount * 3; i++)
      {
        this->points[i] = points[i];
        this->normals[i] = normals[i];
      }

      this->indices = new unsigned int[indexCount];
      for (size_t i = 0; i < indexCount; i++)
      {
        this->indices[i] = indices[i];
      }
    }
  };

} // namespace ar

#endif // _ARPOLYMESH_H
