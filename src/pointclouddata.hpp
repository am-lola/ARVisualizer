#ifndef _ARPOINTCLOUDDATA_H
#define _ARPOINTCLOUDDATA_H

namespace ar
{
  enum PointCloudDataType
  {
    PCL_PointXYZ,
    PCL_PointXYZRGBA
  };

  struct PointCloudData
  {
    const void* pointData;    // Points xyzw (w is ignored, easier because pcl::PointXYZ is aligned to 4 floats).
    unsigned long numPoints;  // Number of points in the array.
    PointCloudDataType type;  // Format points are stored in.

    // Creates a capsule. Centers are expected to be ordered: x, y, z
    PointCloudData(const void* data, unsigned long count, PointCloudDataType dataType)
    {
      pointData = data;
      numPoints = count;
      type = dataType;
    }

  };

} // namespace ar

#endif // _ARPOINTCLOUDDATA_H
