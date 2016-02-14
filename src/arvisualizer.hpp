#ifndef _ARVISUALIZER_H
#define _ARVISUALIZER_H

#include "color.hpp"
#include <map>
#include <vector>

namespace ar
{

// Number of times to subdivide icosphere meshes (larger == smoother spheres)
// This should be a non-negative number between 0 and 5. Smaller is faster.
#define ICOSPHERE_SUBDIV_LEVELS 2

// Number of vertices around major axis to generate on UV sphere-based meshes.
// This should be a non-negative even number. Smaller is faster, larger is smoother.
#define UVSPHERE_RESOLUTION 16

class Renderer;

typedef unsigned int mesh_handle;
typedef unsigned int ui_element_handle;

/*
  The main interface to the AR Visualization library
*/

class ARVisualizer
{
public:
  ARVisualizer();
  ~ARVisualizer();

  // Begins rendering
  void Start(int width, int height);
  void Start();

  // Stops all rendering activity
  void Stop();

  // True if the visualizer has been started
  bool IsRunning();

  // Updates video texture with (RGB24) data in pixels    // TODO: determine if any other pixel formats need to be supported
  void NotifyNewVideoFrame(int width, int height, unsigned char* pixels);

  // Updates the camera parameters used for rendering. Values should correspond
  // as closely as possible to the physical position & orientation of the camera
  //    Position: Position of the camera in world-coordinates
  //    Forward:  Vector pointing in the direction the camera is facing
  //    Up:       Orthogonal to Forward, defines the vertical axis for the camera
  void SetCameraPose(double position[3], double forward[3], double up[3]);

  //    Position:    Position of the camera in world-coordinates
  //    Orientation: Rotation matrix describing the current orientation of the camera
  void SetCameraPose(double position[3], double orientation[3][3]);

  // Updates the projection based on the given camera matrix
  void SetCameraIntrinsics(double camera_matrix[3][3]);

  mesh_handle AddTriangle(double vertexPositions[3][3], Color color);

  mesh_handle AddQuad(double center[3], double normal[3], double width, double height, Color color);

  mesh_handle AddSphere(double center[3], double radius, Color color);

  mesh_handle AddCapsule(double center1[3], double center2[3], double radius, Color color);

  //    Transformation: Column-major 3x3 matrix with orthogonal axes as columns, ordered by descending size for best results.
  mesh_handle AddEllipsoid(float* center, float* transformation, double radius, Color color);

  void Remove(mesh_handle handle);

  void RemoveAll();

  enum PointCloudDataType
  {
    PCL_PointXYZ,
    PCL_PointXYZRGBA
  };

  // Draw a point cloud
  //    Points: Points xyzw (w is ignored, easier because pcl::PointXYZ is aligned to 4 floats).
  //    NumPoints: Number of points in the array.
  void DrawPointCloud(const void* pointData, unsigned long numPoints, PointCloudDataType dataType);

  struct Voxel
  {
    float center[3];
    float color[4];
    float size;
  };

  void DrawVoxels(const Voxel* voxels, unsigned long numVoxels);

  ui_element_handle AddButton(const char* text);
  ui_element_handle AddSliderFloat(const char* text, float min, float max, float value = 0.0f);
  ui_element_handle AddSliderFloat2(const char* text, float min, float max, const float* values = NULL);
  ui_element_handle AddSliderFloat3(const char* text, float min, float max, const float* values = NULL);
  ui_element_handle AddSliderFloat4(const char* text, float min, float max, const float* values = NULL);
  ui_element_handle AddSliderAngle(const char* text, float minDegree, float maxDegree, float valueRadians = 0.0f);
  ui_element_handle AddSliderInt(const char* text, int min, int max, int value = 0);
  ui_element_handle AddSliderInt2(const char* text, int min, int max, const int* values = NULL);
  ui_element_handle AddSliderInt3(const char* text, int min, int max, const int* values = NULL);
  ui_element_handle AddSliderInt4(const char* text, int min, int max, const int* values = NULL);
  ui_element_handle AddCheckBox(const char* text, bool checked = false);
  ui_element_handle AddFloatRange(const char* text, float speed = 1.0f, float min = 0.0f, float max = 0.0f, float lower = 0.0f, float upper = 0.0f);

  bool GetButtonState(ui_element_handle handle);
  float GetSliderFloatValue(ui_element_handle handle) const;
  void GetSliderFloatValues(ui_element_handle handle, float* values) const;
  int GetSliderIntValue(ui_element_handle handle) const;
  void GetSliderIntValues(ui_element_handle handle, int* values) const;
  bool GetCheckBoxState(ui_element_handle handle) const;
  bool GetFloatRangeValues(ui_element_handle handle, float& lower, float& upper) const;

private:
  Renderer* _renderer;

  // All of this should be separated and hidden from this interface
  struct UIElement
  {
    enum Type
    {
      Button,
      SliderFloat,
      SliderInt,
      SliderAngle,
      CheckBox,
      FloatRange,
    };

    UIElement() { }

    UIElement(const Type& type, size_t index)
      : _type(type), _index(index)
    { }

    Type _type;
    size_t _index;
  };

  struct Button
  {
    std::string _text;
    bool _pressed;
  };

  template <typename T>
  struct Slider
  {
    Slider(const std::string& text, T min, T max, const T* values, int numValues)
      : _text(text), _min(min), _max(max), _numValues(numValues)
    {
      for (int i = 0; i < numValues; i++)
        _values[i] = 0;

      if (values != NULL)
      {
        for (int i = 0; i < numValues; i++)
          _values[i] = values[i];
      }
    }

    std::string _text;
    T _min;
    T _max;

    T _values[4];
    int _numValues;
  };

  struct CheckBox
  {
    std::string _text;
    bool _checked;
  };

  struct FloatRange
  {
    std::string _text;
    float _speed;
    float _min;
    float _max;
    float _lowerValue;
    float _upperValue;
    bool _dirty;
  };

  void renderExternGUI();
  ui_element_handle addUIElement(UIElement::Type type, size_t index);

  std::vector<Button> _buttons;
  std::vector<Slider<int> > _intSliders;
  std::vector<Slider<float> > _floatSliders;
  std::vector<CheckBox> _checkBoxes;
  std::vector<FloatRange> _floatRanges;
  std::map<ui_element_handle, UIElement> _uiElements;

  ui_element_handle _nextUIElementHandle;
};

} // namespace ar

#endif // _ARVISUALIZER_H
