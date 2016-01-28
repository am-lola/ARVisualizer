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

  void Update(float deltaTime);
  void RenderGUI();
  void Reset();

  glm::mat4 GetViewMatrix() const;

  void SetPosition(const glm::vec3& position);
  void SetForwardAndUp(const glm::vec3& forward, const glm::vec3& up);

private:

  void OnMouseMove(double xpos, double ypos);
  void OnMouseButton(int button, int action, int mods);
  void OnKey(int key, int scancode, int action, int mods);
  void OnScroll(double offset);

  glm::vec3 _position; // where the camera is
  glm::vec3 _forward;  // where the camera is looking
  glm::vec3 _right;    // right vector
  glm::vec3 _up;       // "up" in world space

  glm::vec3 _rot;      // yaw, pitch, roll (euler angles)

  double _prevMouseX = 0;
  double _prevMouseY = 0;
  bool _mousePressed = false;

  int _movementForward = 0;
  int _movementSide = 0;
  int _movementUp = 0;
  float _movementSpeed = 10.0f;
  float _sensitivity = 1.0f;
};

}

#endif // _CAMERA_H