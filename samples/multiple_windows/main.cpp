#include <iostream>
#include "ARVisualizer.hpp"

/*
  A small sample application to show basic usage of ARVisualizer

  This sample is just a test to ensure using multiple visualizers at once works.
*/
int main(void)
{
  ar::ARVisualizer visualizer1;
  ar::ARVisualizer visualizer2;
  ar::ARVisualizer visualizer3;

  visualizer1.Start(1024, 768);
  visualizer2.Start(512, 512);
  visualizer3.Start(256, 256);

  visualizer2.Stop();
  visualizer3.Stop();

  visualizer3.Start(256, 256);
  visualizer2.Start(128, 128);

  visualizer1.Stop();
  visualizer2.Stop();
  visualizer3.Stop();
}
