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
    // Points xyzw (w is ignored, easier because pcl::PointXYZ is aligned to 4 floats).
    const void* pointData;

    // Number of points in the array.
    unsigned long numPoints;

    // Point format
    PointCloudDataType type;

    // Constant color for the cloud
    Color color;

    PointCloudData() : pointData(0), numPoints(0), type(PCL_PointXYZ), color() {}

    PointCloudData(PointCloudDataType dataType) : pointData(0), numPoints(0), type(dataType), color() {}

    PointCloudData(PointCloudDataType dataType, Color color) : pointData(0), numPoints(0), type(dataType), color(color) {}

    PointCloudData(const void* data, unsigned long count, PointCloudDataType dataType) : color()
    {
      pointData = data;
      numPoints = count;
      type = dataType;
    }

    PointCloudData(const void* data, unsigned long count, PointCloudDataType dataType, Color color) : color(color)
    {
      pointData = data;
      numPoints = count;
      type = dataType;
    }

  };

} // namespace ar

#endif // _ARPOINTCLOUDDATA_H
