#if ! defined(__DEVICE__)
#define __DEVICE__

#include <string>
#include <renderstate.h>
#include <clearstate.h>
#include <light.h>
#include <gl_loader/gl_loader.h>

namespace Device
{
  void SetCameraPos(const glm::vec3& value);
  void SetWorldMatrix(const glm::mat4& value);
  void SetViewMatrix(const glm::mat4& value);
  void SetProjectionMatrix(const glm::mat4& value);

  void EnableLight(size_t index, bool turnOn);
  void SetLight(size_t index, const Light& value);

  bool Initialise(const std::string& title, int width, int height, bool fullScreen);
  void Clear(const ClearState& clearState, GLenum buffers);
  void Draw(GLenum primitiveType, GLint startVertex, GLint vertexCount, const RenderState& renderState);
  void SwapBuffers();
}

#endif // __DEVICE__
