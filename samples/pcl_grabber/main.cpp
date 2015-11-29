#include <pcl/io/openni_grabber.h>

#include "arvisualizer.hpp"

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
  // Create a pcl::Grabber to get data from sensor
  interface = new pcl::OpenNIGrabber();

  // Create ar::ARVisualizer to send data to
  visualizer = new ar::ARVisualizer();

  // subscribe to grabber's image callback so we get RGB data
  boost::function<void (const boost::shared_ptr<openni_wrapper::Image>&)> f = boost::bind (&image_callback, _1);
  interface->registerCallback (f);

  // start the grabber
  interface->start ();

  // start the visualizer
  visualizer->Start(1024, 768);

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
