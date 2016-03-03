#include <pcl/io/openni_grabber.h>

#include "ARVisualizer.hpp"

/*
  A small sample application to show basic usage of ARVisualizer

  This sample connects to an RGBD sensor via PCL's OpenNI Grapper wrapper,
  and provides any incoming RGB images to the visualizer for rendering.
*/

ar::ARVisualizer* visualizer;
pcl::Grabber* interface;

/*
  This callback is called from PCL every time a new RGB image is available
*/
void image_callback (const boost::shared_ptr<openni_wrapper::Image>& image)
{
  // only send data if the visualizer has started
  if (visualizer && visualizer->IsRunning())
  {
    // allocate space to hold image data
    static unsigned char* img_data = new unsigned char[image->getWidth() * image->getHeight() * 3];

    // copy incoming image
    image->fillRGB(image->getWidth(), image->getHeight(), img_data);

    // send image data to visualizer
    // (img_data is safe to delete after this; visualizer makes its own copy )
    visualizer->NotifyNewVideoFrame(image->getWidth(), image->getHeight(), img_data);
  }
}

int main(void)
{
  // Camera intrinsic parameters for a kinect model 1473 rgb sensor
  // Should be fine for testing most similar sensors, but for best results you may
  // want to calibrate your own camera.
  double camera_matrix[3][3] = {
    5.2921508098293293e+02, 0.0, 3.2894272028759258e+02,
    0.0, 5.2556393630057437e+02, 2.6748068171871557e+02,
    0.0, 0.0, 1.0
  };

  // Create a pcl::Grabber to get data from sensor
  interface = new pcl::OpenNIGrabber("", pcl::OpenNIGrabber::OpenNI_QVGA_30Hz);

  // Create ar::ARVisualizer to send data to
  visualizer = new ar::ARVisualizer();

  // subscribe to grabber's image callback so we get RGB data
  boost::function<void (const boost::shared_ptr<openni_wrapper::Image>&)> f = boost::bind (&image_callback, _1);
  interface->registerCallback (f);

  // start the grabber
  interface->start ();

  // start the visualizer
  visualizer->Start(640, 480);

  // set the camera intrinsic parameters
  visualizer->SetCameraIntrinsics(camera_matrix);

  // everything else is async, so just wait until the user has had enough
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
