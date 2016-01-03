#include "camera.hpp"
#include "common.hpp"
#include "windowmanager/windowevents.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>

#include <imgui.h>

namespace ar
{

Camera::Camera(WindowEvents& windowEvents)
{
  Reset();

  windowEvents.SubscribeEvent(WindowEvents::MouseMove, (std::function<void(double,double)>)(
    [this](double xpos, double ypos)
    {
      this->OnMouseMove(xpos, ypos);
    }));

  windowEvents.SubscribeEvent(WindowEvents::MouseButton,
    [this](int button, int action, int mods)
    {
      this->OnMouseButton(button, action, mods);
    });

  windowEvents.SubscribeEvent(WindowEvents::KeyboardKey,
    [this](int key, int scancode, int action, int mods)
    {
      this->OnKey(key, scancode, action, mods);
    });

  windowEvents.SubscribeEvent(WindowEvents::Scroll, (std::function<void(double,double)>)(
    [this](double xoffset, double yoffset)
    {
      this->OnScroll(yoffset);
    }));
}

void Camera::Update(float deltaTime)
{
  // Don't move the camera if an imgui item is active (e.g. for text input)
  if (ImGui::IsAnyItemActive())
    return;

  _position += _forward * float(_movementForward * _movementSpeed * deltaTime);
  _position += _right * float(_movementSide * _movementSpeed * deltaTime);
  _position += _up * float(_movementUp * _movementSpeed * deltaTime);
}

void Camera::OnMouseMove(double xpos, double ypos)
{
  if (ImGui::IsAnyItemActive() || ImGui::IsMouseHoveringAnyWindow())
    return;

  const double deltaX = xpos - _prevMouseX;
  const double deltaY = ypos - _prevMouseY;

  _prevMouseX = xpos;
  _prevMouseY = ypos;

  if (!_mousePressed)
    return;
  
  const float pi = glm::pi<float>();
  const float twoPi = pi * 2.0f;

  _rot.x -= deltaX * _sensitivity * 0.01f;
  _rot.y += deltaY * _sensitivity * 0.01f;

  // Normalize yaw angle
  if (_rot.x < -twoPi)
    _rot.x += twoPi;
  else if (_rot.x > twoPi)
    _rot.x -= twoPi;

  // Clamp pitch to +-90°. Add a small constant for numerical stability.
  _rot.y = clamp(_rot.y, -pi/2.0f + 0.01f, pi/2.0f - 0.01f);

  glm::mat3 mat = glm::orientate3(_rot);
  _forward = mat[2];
  _right = mat[0];
}

void Camera::OnMouseButton(int button, int action, int mods)
{
  if (action == GLFW_PRESS && (ImGui::IsAnyItemActive() || ImGui::IsMouseHoveringAnyWindow()))
    return;

  if (button != 0)
    return;
  
  if (action == GLFW_PRESS)
    _mousePressed = true;
  else if (action == GLFW_RELEASE)
    _mousePressed = false;
}

void Camera::OnKey(int key, int scancode, int action, int mods)
{
  int keyDown;
  if (action == GLFW_PRESS)
  {
    keyDown = 1;
  }
  else if (action == GLFW_RELEASE)
  {
    keyDown = -1;
  }
  else
  {
    return;
  }

  // Need to catch every key event for this to work
  switch (key)
  {
    case GLFW_KEY_W:
      _movementForward += keyDown;
      break;
    case GLFW_KEY_S:
      _movementForward -= keyDown;
      break;
    case GLFW_KEY_A:
      _movementSide += keyDown;
      break;
    case GLFW_KEY_D:
      _movementSide -= keyDown;
      break;
    case GLFW_KEY_E:
      _movementUp += keyDown;
      break;
    case GLFW_KEY_Q:
      _movementUp -= keyDown;
      break;
  }
}

void Camera::OnScroll(double offset)
{
  if (ImGui::IsAnyItemActive() || ImGui::IsMouseHoveringAnyWindow())
    return;

  // Control movement speed with the mouse wheel
  const float deltaSpeed = (float)offset;
  _movementSpeed = std::max(0.1f, _movementSpeed + deltaSpeed);
}

glm::mat4 Camera::GetViewMatrix() const
{
  return glm::lookAt(
      _position,
      _forward + _position,
      _up
    );
}

void Camera::SetPosition(const glm::vec3& position)
{
  _position = position;
}

void Camera::SetForwardAndUp(const glm::vec3& forward, const glm::vec3& up)
{
  _forward = forward;
  _up = up;
  _right = glm::cross(_forward, _up);
  
  // TODO: Calculate _rot
  // glm::mat3 mat(_right, _up, _forward);
}

void Camera::Reset()
{
  _position = glm::vec3(0.0f, 0.0f, 0.0f);
  _forward  = glm::vec3(0.0f, 0.0f, 1.0f);
  _right    = glm::vec3(1.0f, 0.0f, 0.0f);
  _up       = glm::vec3(0.0f, 1.0f, 0.0f);
  _rot      = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Camera::RenderGUI()
{
  ImGui::Begin("Camera");

  ImGui::Text("Pos: (%.2f, %.2f, %.2f)", _position.x, _position.y, _position.z);
  ImGui::Text("Rot: (%.2f, %.2f, %.2f)", _rot.x, _rot.y, _rot.z);
  ImGui::Text("Movement speed: %.2f", _movementSpeed);

  ImGui::Separator();

  ImGui::PushItemWidth(-100);

  static float sensitivity = 1.0f;

  sensitivity = _sensitivity;
  ImGui::SliderFloat("Sensitivity", &sensitivity, 0.2f, 10.0f);
  _sensitivity = sensitivity;

  if (ImGui::Button("Reset View"))
    Reset();

  ImGui::End();
}

}
