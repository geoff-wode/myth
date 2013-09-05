#if ! defined(__DEVICE__)
#define __DEVICE__

#include <string>
#include <renderstate.h>
#include <clearstate.h>
#include <gl_loader/gl_loader.h>

struct Device
{
  // Shader variables that are globally accessible to all shaders...
  struct ShaderVariables
  {
    glm::mat4 WorldMatrix;                // Model- to view-space transform.
    glm::vec3 CameraPos;                  // World coordinate of the "camera".
    glm::mat4 ViewMatrix;                 // View- to projection-space transform.
    glm::mat4 ProjectionMatrix;           // Projection- to screen-space transform.
  } shaderVars;

  const ClearState& GetClearState() const;
  const RenderState& GetRenderState() const;

  bool Initialise(const std::string& title, int width, int height, bool fullScreen);
  void Clear(const ClearState& clearState, GLenum buffers);
  void Draw(GLenum primitiveType, GLint startVertex, GLint vertexCount, const RenderState& renderState);
  void DrawIndexed(GLenum primitiveType, size_t indexCount, GLenum indexType, size_t bufferOffset, const RenderState& renderState);
  void SwapBuffers();
};

#endif // __DEVICE__
