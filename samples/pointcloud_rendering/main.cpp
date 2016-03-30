#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/openni_grabber.h>
#include "ARVisualizer.hpp"

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
  // detect all OpenNI devices & print some info about them
  openni_wrapper::OpenNIDriver::getInstance().updateDeviceList();
  unsigned int nDevices = openni_wrapper::OpenNIDriver::getInstance().getNumberDevices();
  if (nDevices == 0)
  {
    std::cout << "No compatible OpenNI devices detected! Exiting..." << std::endl;
    return 0;
  }

  std::cout << nDevices << " OpenNI Devices Detected:" << std::endl;;
  for (size_t i = 0; i < nDevices; i++)
  {
    auto device = openni_wrapper::OpenNIDriver::getInstance().getDeviceByIndex(i);
    auto rgbOutputMode = device->getDefaultImageMode();
    auto depthOutputMode = device->getDefaultDepthMode();

    std::cout << std::endl;
    std::cout << "\t" << "Device " << i << ": " << device->getVendorName() << " " << device->getProductName() << std::endl;
    std::cout << "\t-------------------------------------------------------" << std::endl;

    std::cout << "\t" << "Serial:" << device->getSerialNumber() << std::endl;
    std::cout << "\t" << "Default Image Output Mode:" << std::endl;
    std::cout << "\t\t" << "Width: " << rgbOutputMode.nXRes << "px, Height: " << rgbOutputMode.nYRes << "px, Frame Rate: " << rgbOutputMode.nFPS << " fps" << std::endl;
    std::cout << "\t" << "Default Depth Output Mode:" << std::endl;
    std::cout << "\t\t" << "Width: " << depthOutputMode.nXRes << "px, Height: " << depthOutputMode.nYRes << "px, Frame Rate: " << depthOutputMode.nFPS << " fps" << std::endl;
  }

  std::cout << "Opening first device..." << std::endl;
  // Create a pcl::Grabber to get data from sensor (using first detected device by default)
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
