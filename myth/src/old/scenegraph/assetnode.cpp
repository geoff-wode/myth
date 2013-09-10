#include <asset.h>
#include <debug.h>
#include <device.h>
#include <texture.h>
#include <buffers.h>
#include <scenegraph/scene.h>
#include <scenegraph/assetnode.h>
#include <gl_loader/gl_loader.h>
#include <boost/make_shared.hpp>

//--------------------------------------------------------

AssetNode::AssetNode(boost::shared_ptr<Asset> asset)
  : asset(asset)
{
}

//--------------------------------------------------------
void AssetNode::Render(Scene* const scene)
{
  RenderState renderState = scene->device->GetRenderState();

  renderState.vao = asset->vao;
  //renderState.shader = asset->shader;

  renderState.textureUnits[0].active = true;
  renderState.textureUnits[0].texture = asset->texture;

  if (asset->indexBuffer)
  {
    scene->device->DrawIndexed(asset->drawType, asset->drawVertexCount, asset->indexBuffer->indexType, 0, renderState);
  }
  else
  {
    scene->device->Draw(asset->drawType, asset->drawFirstVertex, asset->drawVertexCount, renderState);
  }

  SceneNode::Render(scene);
}
