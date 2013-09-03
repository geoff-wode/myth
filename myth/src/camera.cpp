#include <camera.h>
#include <memory>

Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
  : Up(up), Forward(forward), Right(right),
    ViewMatrix(viewMatrix), PerspectiveMatrix(perspectiveMatrix),
  FoV(fov),
  AspectRatio(aspectRatio),
  NearClip(nearClip),
  FarClip(farClip),
  Yaw(0), Pitch(0),
  Position(0)
{
}

void Camera::Update()
{
  orientation = glm::rotate(glm::mat4(), Pitch, glm::vec3(1,0,0));
  orientation = glm::rotate(orientation, Yaw, glm::vec3(0,1,0));
  viewMatrix = orientation * glm::translate(-Position);
  perspectiveMatrix = glm::perspective(FoV, AspectRatio, NearClip, FarClip);

  const glm::mat4 invOrientation(glm::inverse(orientation));
  up = (glm::vec3)(invOrientation * glm::vec4(0,1,0,0));
  forward = (glm::vec3)(invOrientation * glm::vec4(0,0,-1,0));
  right = (glm::vec3)(invOrientation * glm::vec4(1,0,0,0));
}
