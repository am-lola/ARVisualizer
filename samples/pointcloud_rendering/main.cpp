#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/openni_grabber.h>
#include "arvisualizer.hpp"

ar::ARVisualizer* visualizer;
ar::PointCloudData pointCloud(ar::PCL_PointXYZ);
ar::mesh_handle cloud_handle;
pcl::Grabber* interface;

typedef pcl::PointXYZ PointT;

void cloud_cb (const pcl::PointCloud<PointT>::ConstPtr& cloud)
{
  // only send data if the visualizer has started
  if (visualizer && visualizer->IsRunning())
  {
    const PointT* data = &cloud->points[0];
    pointCloud.pointData = reinterpret_cast<const void*>(data);
    pointCloud.numPoints = cloud->size();
    visualizer->Update(cloud_handle, pointCloud); // give the visualizer the new points
  }
}

int main(void)
{
  // Create a pcl::Grabber to get data from sensor
  interface = new pcl::OpenNIGrabber("", pcl::OpenNIGrabber::OpenNI_QVGA_30Hz);

  // Create ar::ARVisualizer to send data to
  visualizer = new ar::ARVisualizer();

  boost::function<void (const pcl::PointCloud<PointT>::ConstPtr&)> f = boost::bind(&cloud_cb, _1);
  interface->registerCallback(f);

  // start the grabber
  interface->start();

  // start the visualizer
  visualizer->Start(1024, 768);

  // add an empty point cloud to visualizer, which we will upate each frame
  cloud_handle = visualizer->Add(pointCloud);

  std::cout << "Press enter to exit..." << std::endl;
  std::cin.get();
  std::cout << "Shutting down..." << std::endl;

  // shutdown pcl::Grabber
  interface->stop ();
  // shutdown visualizer
  visualizer->Stop();

  // cleanup
  delete(interface);
  delete(visualizer);
}
