#if ! defined(__CAMERA__)
#define __CAMERA__

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera
{
public:
  Camera(float fov, float aspectRatio, float nearClip, float farClip);

  void Update(float elapsedMS);

  void SetSpeed(float value);
  void MoveLeft();
  void MoveRight();
  void MoveForward();
  void MoveBackward();
  void MoveUp();
  void MoveDown();

  void Pitch(float value);
  void Yaw(float value);

  const glm::vec3& Up;
  const glm::vec3& Forward;
  const glm::vec3& Right;
  const glm::mat4& ViewMatrix;
  const glm::mat4& PerspectiveMatrix;

  glm::vec3 Position;

private:
  float fov;
  float aspectRatio;
  float nearClip;
  float farClip;
  float yaw;
  float pitch;

  glm::vec3 up;
  glm::vec3 forward;
  glm::vec3 right;
  glm::mat4 viewMatrix;
  glm::mat4 perspectiveMatrix;
  glm::mat4 orientation;
  float speed;
  glm::vec3 movement;
};

#endif // __CAMERA__
