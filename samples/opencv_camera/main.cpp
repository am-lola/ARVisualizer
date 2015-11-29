#include <opencv2/opencv.hpp>
#include "arvisualizer.hpp"

/*
  A small sample application to show basic usage of ARVisualizer

  This sample just uses OpenCV to connect to the default camera on the
  system, and then display the resulting image in the visualizer.
*/


/*
  Copies the raw data from the given cv::Mat to dst_array
  Based on the sample code from: docs.opencv.org/2.4/doc/tutorials/core/how_to_scan_images/how_to_scan_images.html

  Returns True on success, False if the conversion could not be performed

  NOTE: dst_array must already be allocated with enough space to store the image!
*/
bool Mat2Arr(cv::Mat inputImage, unsigned char* dst_array)
{
  if (inputImage.depth() != CV_8U)
  {
    std::cout << "ERROR: Expected input image type: CV_8U! Got cv type: " << inputImage.depth() << std::endl;
    return false;
  }

  int channels = inputImage.channels();
  int nRows = inputImage.rows;
  int nCols = inputImage.cols * channels;

  if (inputImage.isContinuous())
  {
    nCols *= nRows;
    nRows = 1;
  }

  int p = 0;
  for (int i = 0; i < nRows; i++)
  {
    uint8_t* rowPtr = inputImage.ptr<uchar>(i);
    for (int j = 0; j < nCols; j += channels)
    {
      // note the order here, because the cv::Mat is BGR and we need RGB
      dst_array[p + 0] = rowPtr[j + 2]; // R
      dst_array[p + 1] = rowPtr[j + 1]; // G
      dst_array[p + 2] = rowPtr[j + 0]; // B

      p += 3;
    }
  }

  return true;
}

int main(void)
{
  ar::ARVisualizer visualizer;
  cv::VideoCapture cvcapture;
  unsigned char* frame_rgb; // space for this is allocated later once we have at least one video frame

  bool frame_rgb_ready = false;

  // Start the visualizer!
  visualizer.Start(1024, 768);

  if (!cvcapture.open(0))
  {
    std::cout << "Could not open camera!" << std::endl;
    return 0;
  }

  while(1)
  {
    // Get a new image frame from the camera
    cv::Mat frame_bgr;
    cvcapture >> frame_bgr;

    // If the frame was empty, the camera is gone and we should stop
    if ( frame_bgr.empty() )
    {
      break;
    }

    // if we haven't allocated space for our RGB image, yet, do so now
    if (!frame_rgb_ready)
    {
      std::cout << "Allocating space for RGB data:" << std::endl << "\t" <<
                   frame_bgr.size().width << "x" <<
                   frame_bgr.size().height << " pixels, " <<
                   frame_bgr.channels() << " color channels ==> " <<
                   frame_bgr.size().width * frame_bgr.size().height * frame_bgr.channels() <<
                   " total values" << std::endl;

      frame_rgb = new unsigned char[frame_bgr.size().width * frame_bgr.size().height * frame_bgr.channels()];
      frame_rgb_ready = true;
    }

    // convert from BGR cv::Mat to RGB uchar*
    if (Mat2Arr(frame_bgr, frame_rgb))
    {
      // Send data to the visualizer
      visualizer.NotifyNewVideoFrame(frame_bgr.size().width, frame_bgr.size().height, frame_rgb);
    }

    // show original image in an OpenCV window for comparison (colors in both should be the same)
    cv::imshow("OpenCV Cam", frame_bgr);

    if (cv::waitKey(10) == 'q') break;
  }

  visualizer.Stop();
  return 0;
}
