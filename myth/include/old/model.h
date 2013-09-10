#if ! defined(__MODEL__)
#define __MODEL__

#include <vector>
#include <device.h>
#include <buffers.h>
#include <texture.h>
#include <shader.h>
#include <renderstate.h>
#include <gl_loader/gl_loader.h>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

struct Material
{
  boost::shared_ptr<Texture2D> texture;
};

struct Mesh
{
  GLuint vao;
  size_t indexCount;
  size_t materialIndex;
  boost::shared_ptr<VertexBuffer> vertexBuffer;
  boost::shared_ptr<IndexBuffer> indexBuffer;
};

// Represents a renderable geometric object.
class Model
{
public:
  Model();
  ~Model();
  bool Load(const char* const name);
  void Render(Device* device);

private:
  std::vector<Mesh> meshes;
  std::vector<Material> materials;
};

#endif // __MODEL__
