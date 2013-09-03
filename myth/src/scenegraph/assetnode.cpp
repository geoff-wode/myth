#include <asset.h>
#include <device.h>
#include <sampler.h>
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
  RenderState renderState = Device::GetRenderState();

  renderState.vao = asset->vao;
  renderState.shader = asset->shader;

  renderState.textureUnits[0].active = true;
  renderState.textureUnits[0].sampler = asset->sampler;
  renderState.textureUnits[0].texture = asset->texture;

  Device::Draw(asset->drawType, asset->drawFirstVertex, asset->drawVertexCount, renderState);

  SceneNode::Render(scene);
}
