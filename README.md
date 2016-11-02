# Awesome Robot Visualizer

A small visualization library

# Build Instructions

Once the [minimum necessary dependencies](#dependencies) have been installed, you should be able to build and install the library without any trouble using CMake.

```bash
    cd <ARVisulizer-root-dir>
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
```

To build the documentation, ensure you have the necessary [dependencies](#dependencies), then enable the `documentation` target by setting `ARVIS_BUILD_DOCS` to True, using an application like ccmake, cmake-gui, or directly from the commandline:

```bash
    cmake -DARVIS_BUILD_DOCS=True ..
    make documentation
```

The generated documentation will be in `<build-directory>/docs`

Similarly, if you wish NOT to build the samples (which are enabled by default), you can set `ARVIS_BUILD_SAMPLES` to False.

# Usage

After [building and installing](#build-instructions) the library, you should be able to use it without any trouble in other CMake-based projects. Use `find_package(am2b-arvis CONFIG REQUIRED)` to get the paths necessary for building:

    am2b-arvis_LIBRARY      :  Path to the shared library object you need to link against
    am2b-arvis_LIBRARY_DIR  :  Path to the directory containing the shared library
    am2b-arvis_INCLUDE_DIR  :  Path to the header files you need to compile against

Your CMakeLists.txt should probably look something like this:

```cmake
    cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
    
    project(myProject)
    
    find_package(am2b-arvis CONFIG REQUIRED)
    
    add_executable(myProgram main.cpp)
    target_link_libraries(myProgram ${am2b-arvis_LIBRARY})
```

The ARVisualizer class can then be found by including `<am2b-arvis/ARVisualizer.hpp>`:

```c++
    #include <am2b-arvis/ARVisualizer.hpp>

    int main(void)
    {
        ar::ARVisualizer visualizer;
        visualizer.Start();

        visualizer.Add(ar::Sphere( ... ));

        ...
    }
```

The visualizer supports some mouse and keyboard shortcuts which can be applied to any visualization instance:

          Left Mouse : Rotate camera around its current position
         Right Mouse : Pan camera (movement relative to image plane)
    Left+Right Mouse : Rotate camera around a point in space (set with Orbit Radius in camera GUI)
        Middle Mouse : Move camera forward and backward (movement relative to image plane)
         Mouse Wheel : Camera Zoom in and out
           'W/S/A/D' : Move camera horizontally
               'Q/E' : Move camera vertically
             'Z/Y/X' : Roll camera around its forward axis
                 '1' : Render meshes as shaded Triangles (default)
                 '2' : Render meshes as wireframes
                 '3' : Render meshes as points (one point per vertex)

# Dependencies

To [compile the library](#build-instructions), you need Python, [GLM](http://glm.g-truc.net/) (which you can apt-get install), and [GLFW3](http://www.glfw.org/) (which you might need to build from source depending on your platform).

To compile the samples, you will also need [PCL](http://pointclouds.org/) and [OpenCV](http://opencv.org/)

To build the documentation you will also need [cldoc](https://jessevdk.github.io/cldoc/)

A script is provided in `/scripts/install_dependencies.sh` which can be used to automatically fetch and install the necessary dependencies on Ubuntu-based platforms:

    install_dependencies.sh
        Installs the minimum dependencies necessary to build the library

    install_dependencies.sh all
        Installs all dependencies necessary to build the library, the samples, and the documentation
