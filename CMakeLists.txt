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
        src/mesh/mesh.*pp)
    add_library(arvisualizer SHARED ${libarvis_src})
    set_target_properties(arvisualizer PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    target_link_libraries(arvisualizer ${GLFW_LIBRARIES})
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders" DESTINATION "${CMAKE_BINARY_DIR}")
endif()

if (ARVIS_BUILD_SAMPLES)
    find_package(PCL 1.2 REQUIRED)
    include_directories(${PCL_INCLUDE_DIRS})
    add_executable(simple_video samples/simple_video/main.cpp)
    target_link_libraries(simple_video arvisualizer ${PCL_LIBRARIES})
endif()
