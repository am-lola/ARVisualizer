#ifndef _ARPOINTCLOUDDATA_H
#define _ARPOINTCLOUDDATA_H

#include "Color.hpp"

namespace ar
{
  enum PointCloudDataType
  {
    PCL_PointXYZ,
    PCL_PointXYZRGBA
  };

  struct PointCloudData
  {
    // For PCL_PointXYZ: Points xyzw floats (w is ignored, easier because pcl::PointXYZ is aligned to 4 floats)
    // For PCL_PointXYZRGBA: Points xyzw floats + 4 uint8 bgra + 12 bytes padding
    const void* pointData;

    // Number of points in the array.
    unsigned long numPoints;

    // Point format (const to discourage changing the type at some later point)
    const PointCloudDataType type;

    // Constant color for the cloud
    Color color;

    PointCloudData(PointCloudDataType dataType) :
      pointData(0),
      numPoints(0),
      type(dataType),
      color()
    {}

    PointCloudData(PointCloudDataType dataType, Color color) :
      pointData(0),
      numPoints(0),
      type(dataType),
      color(color) {}

    PointCloudData(const void* data, unsigned long count, PointCloudDataType dataType) :
      type(dataType),
      color()
    {
      pointData = data;
      numPoints = count;
    }

    PointCloudData(const void* data, unsigned long count, PointCloudDataType dataType, Color color) :
      type(dataType),
      color(color)
    {
      pointData = data;
      numPoints = count;
    }
  };

} // namespace ar

#endif // _ARPOINTCLOUDDATA_H
