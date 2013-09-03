#if ! defined(__CAMERA__)
#define __CAMERA__

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera
{
public:
  Camera(float fov, float aspectRatio, float nearClip, float farClip);
  void Update();

  const glm::vec3& Up;
  const glm::vec3& Forward;
  const glm::vec3& Right;
  const glm::mat4& ViewMatrix;
  const glm::mat4& PerspectiveMatrix;

  float FoV;
  float AspectRatio;
  float NearClip;
  float FarClip;
  float Yaw;
  float Pitch;
  glm::vec3 Position;

private:
  glm::vec3 up;
  glm::vec3 forward;
  glm::vec3 right;
  glm::mat4 viewMatrix;
  glm::mat4 perspectiveMatrix;
  glm::mat4 orientation;
};

#endif // __CAMERA__
