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

  if(_rolling)
  {
    Roll(_rolling);
  }
}

void Camera::UpdateProjection()
{
  if (_useCameraIntrinsics)
  {
    _projectionMatrix = glm::mat4(
      _cameraMatrix[0][0] / _cameraMatrix[0][2], 0,        0,       0,
      0, _cameraMatrix[1][1] / _cameraMatrix[1][2],        0,       0,
      0, 0, -(_farClip + _nearClip) / (_farClip - _nearClip),    -1.0,
      0, 0, (-2.0 * _farClip * _nearClip) / (_farClip - _nearClip), 0
    );
  }
  else
  {
    _projectionMatrix = glm::perspective(
       _fov * _zoom,
       _aspect,
       _nearClip,
       _farClip
     );
  }
}

void Camera::SetIntrinsics(double camera_matrix[3][3])
{
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      _cameraMatrix[i][j] = camera_matrix[i][j];
    }
  }

  _useCameraIntrinsics = true;
  UpdateProjection();
}

void Camera::RenderGUI()
{
  ImGui::Begin("Camera");

  ImGui::Text("Pos: (%.2f, %.2f, %.2f)", _position.x, _position.y, _position.z);
  ImGui::Text("Fwd: (%.2f, %.2f, %.2f)",  _forward.x,  _forward.y,  _forward.z);
  ImGui::Text(" Up: (%.2f, %.2f, %.2f)",       _up.x,       _up.y,       _up.z);

  ImGui::Separator();
  ImGui::PushItemWidth(-100);

  static float sensitivity;
  static float movementSpeed;
  static float orbitRadius;
  sensitivity   = _sensitivity;
  movementSpeed = _movementSpeed;
  orbitRadius   = _orbitRadius;

  ImGui::SliderFloat("Sensitivity", &sensitivity, 0.2f, 10.0f);
  _sensitivity = sensitivity;
  ImGui::SliderFloat("Movement Speed", &movementSpeed, 0.2f, 20.0f);
  _movementSpeed = movementSpeed;
  ImGui::SliderFloat("Orbit Radius", &orbitRadius, 0.5f, 100.0f);
  _orbitRadius = orbitRadius;

  ImGui::Separator();
  static int e = 0;
  ImGui::Text("Scroll Wheel:");
  ImGui::RadioButton("FOV", &e, 0); ImGui::SameLine();
  ImGui::RadioButton("Speed", &e, 1);
  _scrollWheelFunction = e == 0 ? ScrollWheelFunction::ChangeFOV : ScrollWheelFunction::ChangeMovementSpeed;

  ImGui::Separator();

  if (ImGui::Button("Reset View"))
    Reset();

  ImGui::End();
}

void Camera::Reset()
{
  // Don't move the camera if an imgui item is active (e.g. for text input)
  if (ImGui::IsAnyItemActive())
    return;

  _position = _basePosition;
  _forward  = _baseForward;
  _up       = _baseUp;
  _right    = glm::cross(_up, _forward);
  _zoom     = _baseZoom;
}

glm::mat4 Camera::GetViewMatrix() const
{
  return glm::lookAt(
      _position,
      _forward + _position,
      _up
    );
}

glm::mat4 Camera::GetProjectionMatrix() const
{
  return _projectionMatrix;
}

void Camera::SetPosition(const glm::vec3& position)
{
  _position = position;
  _basePosition = position;
}

void Camera::SetForwardAndUp(const glm::vec3& forward, const glm::vec3& up)
{
  _forward = forward;
  _up = up;
  _right = glm::cross(_up, _forward);

  _baseForward = forward;
  _baseUp = up;
}

void Camera::SetAspectRatio(float ratio)
{
  _aspect = ratio;
  UpdateProjection();
}

void Camera::OnMouseMove(double xpos, double ypos)
{
  if (ImGui::IsAnyItemActive() || ImGui::IsMouseHoveringAnyWindow())
    return;

  const double deltaX = xpos - _prevMouseX;
  const double deltaY = ypos - _prevMouseY;

  _prevMouseX = xpos;
  _prevMouseY = ypos;

  if (_leftMousePressed && _rightMousePressed)
  {
    Orbit(deltaX, deltaY);
  }
  else if (_leftMousePressed)
  {
    Rotate(deltaX, deltaY);
  }
  else if (_rightMousePressed)
  {
    Pan(deltaX, deltaY);
  }

}

void Camera::OnMouseButton(int button, int action, int mods)
{
  if (action == GLFW_PRESS && (ImGui::IsAnyItemActive() || ImGui::IsMouseHoveringAnyWindow()))
    return;

  if (action == GLFW_PRESS)
  {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
      _leftMousePressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
      _rightMousePressed = true;
    }
  }
  else if (action == GLFW_RELEASE)
  {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
      _leftMousePressed = false;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
      _rightMousePressed = false;
    }
  }
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
    case GLFW_KEY_R:
      Reset();
      break;
    case GLFW_KEY_Z:
      _rolling -= keyDown;
      break;
    case GLFW_KEY_Y:
      _rolling -= keyDown;
      break;
    case GLFW_KEY_X:
      _rolling += keyDown;
      break;
    default:
      break;
  }
}

void Camera::OnScroll(double offset)
{
  if (ImGui::IsAnyItemActive() || ImGui::IsMouseHoveringAnyWindow())
    return;

  if (_scrollWheelFunction == ScrollWheelFunction::ChangeFOV)
  {
    Zoom(offset);
  }
  else
  {
    // Control movement speed with the mouse wheel
    AlterMovementSpeed(offset);
  }
}

void Camera::Rotate(double dx, double dy)
{
  float rot_hor = dx * _sensitivity * -0.01f;
  float rot_ver = dy * _sensitivity *  0.01f;

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

void Camera::Orbit(double dx, double dy)
{
  glm::vec3 orbitPoint = _forward * _orbitRadius;

  Rotate(dx, dy);

  // Correct position to rotate camera around orbitPoint
  glm::vec3 delta = orbitPoint - (_forward*_orbitRadius);
  _position += delta;
}

void Camera::Pan(double dx, double dy)
{
  // 'up' vector w.r.t. camera
  glm::vec3 cameraUp = glm::cross(_forward, _right);
  glm::vec3 translation = _movementSpeed * 0.001f * ((float)dx * _right + (float)dy * cameraUp);
  _position += translation;
}

void Camera::Roll(double dt)
{
  float rot = dt * _sensitivity *  0.01f;

  // rotate
  _up = glm::rotate(_up, rot, _forward);
  // compute new right vector (necessary to keep horizontal movement & vertical rotation correct)
  _right = glm::cross(_up, _forward);
}

void Camera::Zoom(double dz)
{
  _zoom -= dz * 0.001f;
  UpdateProjection();
}

void Camera::AlterMovementSpeed(double dz)
{
  float deltaSpeed = (float)dz;

  // Change the movement speed exponentially
  if (deltaSpeed > 0.0f)
    deltaSpeed = _movementSpeed * deltaSpeed;
  else
    deltaSpeed = _movementSpeed / (2 * deltaSpeed);

  _movementSpeed = clamp(_movementSpeed + deltaSpeed, 0.1f, 64.0f);
}

} // namespace ar
