cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(am2b-arvis)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

find_package(OpenGL REQUIRED)

include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/extern/stb")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui")

option(ARVIS_BUILD_LIBRARY "Build AR Visualization library" TRUE)
option(ARVIS_BUILD_SAMPLES "Build sample applications" TRUE)
option(ARVIS_BUILD_DOCS    "Build Project Documentation" FALSE)
option(GLFW_STATIC_LINKAGE "Link GLFW3 statically" TRUE)

# Build everything with C++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


# Figure out whether we should be linking against the static or shared version of GLFW3
if (GLFW_STATIC_LINKAGE)
  set(AR_GLFW_LIBS ${GLFW_STATIC_LIBRARIES})
  message(STATUS "Using static GLFW3 Lib")
else()
  set(AR_GLFW_LIBS ${GLFW_LIBRARIES})
  message(STATUS "Using shared GLFW3 Lib")
endif()

if (ARVIS_BUILD_LIBRARY)
    # shader generation scripts
    add_custom_target(
        shader_gen
        COMMAND "python" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/genshaders_cpp.py" "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/" "${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderSources.g.hpp"
        COMMENT "Generating Shader Source Strings..."
    )

    set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++")
    file(GLOB libarvis_src
        src/ARVisualizer.*pp
        src/Renderer.*pp
        src/ImguiRenderer.*pp
        src/LoadShaders.*pp
        src/ShaderProgram.*pp
        src/Material.*pp
        src/Camera.*pp
        src/mesh/Mesh.*pp
        src/mesh/MeshFactory.*pp
        src/windowmanager/WindowManager.*pp
        src/windowmanager/WindowEvents.*pp
        src/windowmanager/GLFWWindowEvents.*pp
        src/ui/ui.cpp
        src/rendering/MeshRendering.*pp
        src/rendering/VideoRendering.*pp
        src/rendering/PointCloudRendering.*pp
        src/rendering/VoxelRendering.*pp
        src/rendering/LineRendering.*pp
        extern/imgui/imgui.cpp
        extern/imgui/imgui_draw.cpp
        extern/imgui/imgui_demo.cpp
    )

    add_library(arvisualizer SHARED ${libarvis_src})
    add_dependencies(arvisualizer shader_gen)
    set_target_properties(arvisualizer PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
    target_link_libraries(arvisualizer ${AR_GLFW_LIBS} ${OPENGL_LIBRARIES} )

    # install target for the visualization library
    set(config_install_dir "lib/cmake/${PROJECT_NAME}")
    set(include_install_dir "include/${PROJECT_NAME}")
    set(lib_install_dir "lib")
    set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

    set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")

    include(CMakePackageConfigHelpers)
    configure_package_config_file(
      "cmake/am2b-arvisConfig.cmake.in"
      "${project_config}"
      INSTALL_DESTINATION "${config_install_dir}"
      PATH_VARS config_install_dir include_install_dir lib_install_dir
    )
    install(TARGETS arvisualizer
      LIBRARY DESTINATION ${lib_install_dir}
    )
    install(FILES
      src/ARVisualizer.hpp
      src/ui.hpp
      src/CircularBuffer.hpp
      src/Delegate.hpp
      DESTINATION ${include_install_dir}
    )
    install(FILES
      src/geometry/Box.hpp
      src/geometry/Capsule.hpp
      src/geometry/Color.hpp
      src/geometry/Ellipsoid.hpp
      src/geometry/Line.hpp
      src/geometry/PointCloudData.hpp
      src/geometry/Polygon.hpp
      src/geometry/PolyMesh.hpp
      src/geometry/Quad.hpp
      src/geometry/Sphere.hpp
      src/geometry/Transform.hpp
      src/geometry/Triangle.hpp
      src/geometry/Voxel.hpp
      DESTINATION ${include_install_dir}/geometry/
    )
    install(FILES
      "${project_config}"
      DESTINATION ${config_install_dir}
    )

endif()

if (ARVIS_BUILD_SAMPLES)
    # PCL Is required for the grabber sample
    find_package(PCL 1.7 REQUIRED COMPONENTS common io)
    include_directories(${PCL_INCLUDE_DIRS})
    add_executable(sample_pcl_grabber samples/pcl_grabber/main.cpp)
    target_link_libraries(sample_pcl_grabber arvisualizer ${PCL_LIBRARIES})

    add_executable(sample_pointcloud_rendering samples/pointcloud_rendering/main.cpp)
    target_link_libraries(sample_pointcloud_rendering arvisualizer ${PCL_LIBRARIES})

    # OpenCV is required for the OpenCV sample
    find_package(OpenCV REQUIRED)
    add_executable(sample_opencv_camera samples/opencv_camera/main.cpp)
    target_link_libraries(sample_opencv_camera arvisualizer ${OpenCV_LIBS})

    # The mesh sample has no additional dependencies
    add_executable(sample_mesh_rendering samples/mesh_rendering/main.cpp)
    target_link_libraries(sample_mesh_rendering arvisualizer)

    add_executable(sample_multiple_windows samples/multiple_windows/main.cpp)
    target_link_libraries(sample_multiple_windows arvisualizer)

    add_executable(sample_user_interface samples/user_interface/main.cpp)
    target_link_libraries(sample_user_interface arvisualizer)
endif()

# Documentation
if (ARVIS_BUILD_DOCS)
    find_program(CLDOC cldoc)
    if( NOT CLDOC )
        message("CLDOC: ${CLDOC}")
        message(FATAL_ERROR "Could not find cldoc! Documentation cannot be built!")
    else()
        add_custom_target(
             documentation
             COMMAND
             ${CLDOC} generate
             -I${CMAKE_CURRENT_SOURCE_DIR}/src
             -I${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui
             --std=c++11
             --
             --basedir ${CMAKE_CURRENT_SOURCE_DIR}/src
             --output ${CMAKE_CURRENT_BINARY_DIR}/docs
             ${CMAKE_CURRENT_SOURCE_DIR}/src/*
             COMMENT "Generating Documentation..."
         )
    endif()
endif()
