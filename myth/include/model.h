#if ! defined(__MODEL__)
#define __MODEL__

#include <vector>
#include <device.h>
#include <modelmesh.h>
#include <texture.h>

class Model
{
public:
  Model();
  ~Model();

  bool Load(const char* const name);
  void Draw(Device* const device);

  const std::vector<boost::shared_ptr<ModelMesh>>& GetMeshes() const { return meshes; }

private:
  std::vector<boost::shared_ptr<ModelMesh>> meshes;
  std::vector<boost::shared_ptr<Texture2D>> textures;
};

#endif // __MODEL__