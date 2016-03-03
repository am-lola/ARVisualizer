# Awesome Robot Visualizer

A small visualization library

## Usage

After building and installing the library (`make`, `sudo make install`, you should be able to use it without any trouble in other CMake-based projects. Use `find_package(am2b-ar CONFIG REQUIRED)` to get the paths necessary for building:

    am2b-ar_LIBRARY      :  Path to the shared library object you need to link against
    am2b-ar_LIBRARY_DIR  :  Path to the directory containing the shared library
    am2b-ar_INCLUDE_DIR  :  Path to the header files you need to compile against

The ARVisualizer class can be found by including `<am2b-ar/ARVisualizer.hpp>`:

    #include <am2b-ar/ARVisualizer.hpp>
    
    int main(void)
    {
        ar::ARVisualizer visualizer;
        visualizer.Start();
        
        visualizer.Add(ar::Sphere( ... ));
        
        ...
    }

The visualizer supports some keyboard shortcuts which can be applied to any visualization instance:

    '1' : Render meshes as shaded Triangles (default)
    '2' : Render meshes as wireframes
    '3' : Render meshes as points (one point per vertex)

# Dependencies

To compile the library, you need Python, [GLM](http://glm.g-truc.net/) (which you can apt-get install), and [GLFW3](http://www.glfw.org/) (which you might need to build from source depending on your platform).

To compile the samples, you will also need [PCL](http://pointclouds.org/) and [OpenCV](http://opencv.org/)

To build the documentation you will also need [cldoc](https://jessevdk.github.io/cldoc/)

A script is provided in `/scripts/install_dependencies.sh` which can be used to automatically fetch and install the necessary dependencies on Ubuntu-based platforms:

    install_dependencies.sh
        Installs the minimum dependencies necessary to build the library
    
    install_dependencies.sh all
        Installs all dependencies necessary to build the library, the samples, and the documentation


