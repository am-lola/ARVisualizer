#include "Camera.hpp"
#include "common.hpp"
#include "windowmanager/WindowEvents.hpp"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>

#include <imgui.h>

namespace ar
{

Camera::Camera(WindowEvents& windowEvents)
{
  Reset();

  windowEvents.GetMouseMoveDelegate() += [this](double xpos, double ypos)
  {
    this->OnMouseMove(xpos, ypos);
  };

  windowEvents.GetMouseButtonDelegate() += [this](int button, int action, int mods)
  {
    this->OnMouseButton(button, action, mods);
  };

  windowEvents.GetKeyboardKeyDelegate() += [this](int key, int scancode, int action, int mods)
  {
    this->OnKey(key, scancode, action, mods);
  };

  windowEvents.GetScrollDelegate() += [this](double xoffset, double yoffset)
  {
    this->OnScroll(yoffset);
  };
}

void Camera::Update(float deltaTime)
{
  // Don't move the camera if an imgui item is active (e.g. for text input)
  if (ImGui::IsAnyItemActive())
    return;

  _position += _forward * (_movementForward * _movementSpeed * deltaTime);
  _position += _right * (_movementSide * _movementSpeed * deltaTime);
  _position += _up * (_movementUp * _movementSpeed * deltaTime);
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

  float rot_hor = deltaX * _sensitivity * -0.01f;
  float rot_ver = deltaY * _sensitivity *  0.01f;

  if (rot_hor != 0)
  {
    // rotate around vertical axis
    _forward = glm::rotate(_forward, rot_hor, _up);
    // compute new right vector (necessary to keep horizontal movement & vertical rotation correct)
    _right = glm::normalize(glm::cross(_up, _forward));
  }

  if (rot_ver != 0)
  {
    // rotate around horizontal axis
    glm::vec3 newForward = glm::rotate(_forward, rot_ver, _right);

    // only take the new vertical rotation if it's within +/- 90 degrees of our vertical axis
    if (glm::angle(glm::cross(newForward, _right), _up) < glm::half_pi<float>())
    {
      _forward = newForward;
    }
  }
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
    default:
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
}

void Camera::Reset()
{
  _position = _basePosition;
  _forward  = _baseForward;
  _up       = _baseUp;
  _right    = glm::cross(_forward, _up);
}

void Camera::RenderGUI()
{
  ImGui::Begin("Camera");

  ImGui::Text("Pos: (%.2f, %.2f, %.2f)", _position.x, _position.y, _position.z);

  ImGui::Separator();
  ImGui::PushItemWidth(-100);

  static float sensitivity;
  static float movementSpeed;
  sensitivity   = _sensitivity;
  movementSpeed = _movementSpeed;

  ImGui::SliderFloat("Sensitivity", &sensitivity, 0.2f, 10.0f);
  _sensitivity = sensitivity;
  ImGui::SliderFloat("Movement Speed", &movementSpeed, 0.2f, 20.0f);
  _movementSpeed = movementSpeed;

  if (ImGui::Button("Reset View"))
    Reset();

  ImGui::End();
}

}
