#include <modelmesh.h>
#include <gl_loader/gl_loader.h>

//---------------------------------------------------------------------

ModelMesh::ModelMesh()
{
}

ModelMesh::~ModelMesh()
{
}

//---------------------------------------------------------------------
void ModelMesh::Draw(Device* const device)
{
  RenderState renderState = device->GetRenderState();
  renderState.vertexArray = vertexArray;
  renderState.textureUnits[0].active = true;
  renderState.textureUnits[0].texture = texture;

  if (indexBuffer)
  {
    device->DrawIndexed(GL_TRIANGLES, indexBuffer->IndexCount, 0, indexBuffer->IndexType, renderState);
  }
  else
  {
    device->Draw(GL_TRIANGLES, vertexBuffer->VertexCount, 0, renderState);
  }
}
