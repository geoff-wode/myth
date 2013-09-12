#if ! defined(__MODEL_MESH__)
#define __MODEL_MESH__

#include <vector>
#include <device.h>
#include <vertexarray.h>
#include <buffers/indexbuffer.h>
#include <buffers/vertexbuffer.h>
#include <boost/shared_ptr.hpp>

// Represents a single set of geometry which uses a single material.
class ModelMesh
{
public:
  ModelMesh();
  virtual ~ModelMesh();

  void SetTexture(boost::shared_ptr<Texture2D> value) { texture = value; }
  void SetVertexArray(boost::shared_ptr<VertexArray> value) { vertexArray = value; }
  void SetVertexBuffer(boost::shared_ptr<VertexBuffer> value) { vertexBuffer = value; }
  void SetIndexBuffer(boost::shared_ptr<IndexBuffer> value) { indexBuffer = value; }

  boost::shared_ptr<Texture2D> GetTexture() const { return texture; }
  boost::shared_ptr<VertexArray>  GetVertexArray() const { return vertexArray; }
  boost::shared_ptr<IndexBuffer>  GetIndexBuffer() const { return indexBuffer; }
  boost::shared_ptr<VertexBuffer> GetVertexBuffer() const { return vertexBuffer; }

  void Draw(Device* const device);

protected:
  boost::shared_ptr<Texture2D> texture;
  boost::shared_ptr<VertexArray> vertexArray;
  boost::shared_ptr<IndexBuffer> indexBuffer;
  boost::shared_ptr<VertexBuffer> vertexBuffer;
};

#endif // __MODEL_MESH__
