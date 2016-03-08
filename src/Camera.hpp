#ifndef _CAMERA_H
#define _CAMERA_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace ar
{

class WindowEvents;

// Basic WASD+Mouse FPS camera
class Camera
{
public:
  Camera(WindowEvents& windowEvents);

  // Updates the camera's current position
  void Update(float deltaTime);
  // Updates the camera's projection matrix
  void UpdateProjection();
  // Sets camera's intrinsic projection parameters (from physical camera calibration)
  void SetIntrinsics(double camera_matrix[3][3]);
  // Renders the camera's GUI controls window
  void RenderGUI();
  // Restores the camera position & orientation to default values
  void Reset();

  // Retrieves the camera view matrix
  glm::mat4 GetViewMatrix() const;
  // Retrieves the camera projection matrix
  glm::mat4 GetProjectionMatrix() const;

  // Sets the camera's position in space
  void SetPosition(const glm::vec3& position);
  // Sets the camera's orientation
  void SetForwardAndUp(const glm::vec3& forward, const glm::vec3& up);
  // Sets the camera's aspect ratio (used for computing the projection matrix)
  void SetAspectRatio(float ratio);

private:

  void OnMouseMove(double xpos, double ypos);
  void OnMouseButton(int button, int action, int mods);
  void OnKey(int key, int scancode, int action, int mods);
  void OnScroll(double offset);

  // Computes new camera orientation from mouse movement
  // @dx X-distance mouse has moved since previous frame
  // @dy Y-distance mouse has moved since previous frame
  void Rotate(double dx, double dy);

  // Computes new camera location from mouse movement
  // @dx X-distance mouse has moved since previous frame
  // @dy Y-distance mouse has moved since previous frame
  void Pan(double dx, double dy);

  // Zooms camera in or out
  void Zoom(double dz);

  float _aspect   = 1.3f;    // aspect ratio
  float _fov      = 45.0f;   // field of view, in degrees
  float _nearClip = 0.1f;    // distance to near clipping plane
  float _farClip  = 10000.0f; // distance to far clipping plane
  float _zoom     = 1.0f;

  glm::mat3 _cameraMatrix;    // camera intrinsic parameters
  glm::mat4 _projectionMatrix;
  bool _useCameraIntrinsics = false;

  glm::vec3 _position; // where the camera is
  glm::vec3 _forward;  // where the camera is looking
  glm::vec3 _right;    // right vector
  glm::vec3 _up;       // "up" in world space

  // default (untransformed) camera pose parameters
  glm::vec3 _basePosition = {0.0f, 0.0f, 0.0f};
  glm::vec3 _baseForward  = {0.0f, 0.0f, 1.0f};
  glm::vec3 _baseUp       = {0.0f, 1.0f, 0.0f};
  float     _baseZoom     = 1.0f;

  double _prevMouseX = 0;
  double _prevMouseY = 0;
  bool _leftMousePressed  = false;
  bool _rightMousePressed = false;

  int _movementForward = 0;
  int _movementSide = 0;
  int _movementUp = 0;
  float _movementSpeed = 10.0f;
  float _sensitivity = 1.0f;
};

}

#endif // _CAMERA_H
