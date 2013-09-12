#if ! defined(__DEVICE__)
#define __DEVICE__

#include <string>
#include <map>
#include <clearstate.h>
#include <renderstate.h>
#include <window.h>
#include <boost/shared_ptr.hpp>
#include <gl_loader/gl_loader.h>
#include <buffers/uniformbuffer.h>
#include <glm/glm.hpp>

class Device
{
public:
  glm::mat4 WorldMatrix;
  glm::mat4 ViewMatrix;
  glm::mat4 ProjectionMatrix;

  Device();
  ~Device();

  ClearState GetClearState() const { return clearState; }
  RenderState GetRenderState() const { return renderState; }
  boost::shared_ptr<Window> GetWindow() const { return mainWindow; }

  void Initialise(bool fullScreen, int backbufferWidth, int backbufferHeight);

  void Clear(GLenum buffers, const ClearState& clearState);

  // Use the given render state to draw primitives.
  void Draw(GLenum primitiveType, size_t vertexCount, size_t vertexIndex, const RenderState& renderState);

  // Use the given render state to draw indexed primitives.
  void DrawIndexed(GLenum primitiveType, size_t indexCount, size_t startIndex, GLenum indexType, const RenderState& renderState);

private:
  boost::shared_ptr<Window> mainWindow;
  boost::shared_ptr<UniformBuffer> globalUniforms;

  ClearState clearState;
  RenderState renderState;
};

#endif // __DEVICE__
