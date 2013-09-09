#include <camera.h>
#include <memory>

Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
  : Up(up), Forward(forward), Right(right),
    ViewMatrix(viewMatrix), PerspectiveMatrix(perspectiveMatrix),
  fov(fov),
  aspectRatio(aspectRatio),
  nearClip(nearClip),
  farClip(farClip),
  yaw(0),
  pitch(0),
  Position(0),
  speed(2.0f),
  movement(0)
{
}

// --------------------------------------------------------
void Camera::SetSpeed(float value) { speed = value; }
void Camera::MoveLeft() { movement.x = -1; }
void Camera::MoveRight() { movement.x = 1; }
void Camera::MoveForward() { movement.z = -1; }
void Camera::MoveBackward() { movement.z = 1; }
void Camera::MoveUp() { movement.y = 1; }
void Camera::MoveDown() { movement.y = -1; }

// --------------------------------------------------------

void Camera::Pitch(float value) { pitch += value; }
void Camera::Yaw(float value) { yaw += value; }

// --------------------------------------------------------

void Camera::Update(float elapsedMS)
{
  const float elapsedSeconds(elapsedMS / 1000.0f);

  // Determine where the local XYZ axes are pointing...
  {
    orientation = glm::rotate(glm::mat4(), pitch, glm::vec3(1,0,0));
    orientation = glm::rotate(orientation, yaw, glm::vec3(0,1,0));

    const glm::mat4 invOrientation(glm::inverse(orientation));
    up = (glm::vec3)(invOrientation * glm::vec4(0,1,0,0));
    forward = (glm::vec3)(invOrientation * glm::vec4(0,0,-1,0));
    right = (glm::vec3)(invOrientation * glm::vec4(1,0,0,0));
  }

  // Move along the local XYZ axes...
  {
    const float velocity(elapsedSeconds * speed);
    Position += (glm::vec3)(velocity * glm::vec4(movement,0) * orientation);
    movement = glm::vec3(0);
  }

  // Compute new matrices...
  viewMatrix = orientation * glm::translate(-Position);
  perspectiveMatrix = glm::perspective(fov, aspectRatio, nearClip, farClip);
}
