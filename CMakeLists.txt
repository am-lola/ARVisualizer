cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(am2b-ar)


find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories(${GLFW_INCLUDE_DIRS})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

option(ARVIS_BUILD_LIBRARY "Build AR Visualization library" TRUE)
option(ARVIS_BUILD_SAMPLES "Build sample applications" TRUE)

if (ARVIS_BUILD_LIBRARY)
    set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++")
    file(GLOB libarvis_src
        src/arvisualizer.*pp
        src/renderer.*pp
        src/loadShaders.*pp
        src/ShaderProgram.*pp
        src/material.*pp
        src/mesh/mesh.*pp
        src/mesh/meshfactory.*pp
        src/windowmanager/windowmanager.*pp)
    add_library(arvisualizer SHARED ${libarvis_src})
    set_target_properties(arvisualizer PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    target_link_libraries(arvisualizer ${GLFW_LIBRARIES})
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders" DESTINATION "${CMAKE_BINARY_DIR}")
endif()

if (ARVIS_BUILD_SAMPLES)
    # PCL Is required for the grabber sample
    find_package(PCL 1.2 REQUIRED)
    include_directories(${PCL_INCLUDE_DIRS})
    add_executable(sample_pcl_grabber samples/pcl_grabber/main.cpp)
    target_link_libraries(sample_pcl_grabber arvisualizer ${PCL_LIBRARIES})

    # OpenCV is required for the OpenCV sample
    find_package(OpenCV REQUIRED)
    add_executable(sample_opencv_camera samples/opencv_camera/main.cpp)
    target_link_libraries(sample_opencv_camera arvisualizer ${OpenCV_LIBS})

    # The mesh sample has no additional dependencies
    add_executable(sample_mesh_rendering samples/mesh_rendering/main.cpp)
    target_link_libraries(sample_mesh_rendering arvisualizer)

    add_executable(sample_multiple_windows samples/multiple_windows/main.cpp)
    target_link_libraries(sample_multiple_windows arvisualizer)
endif()
