#if ! defined(__EFFECT__)
#define __EFFECT__

#include <effectparameter.h>
#include <string>
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include <gl_loader/gl_loader.h>

class Effect
{
public:
  Effect();
  virtual ~Effect();

  bool Load(const char* const name);

  void Use();

  // Return the index for attribute 'name' in the shader.
  static GLint GetAttributeIndex(const char* const name);

  const std::map<std::string, EffectParameter* const>& GetParameters() const;

private:
  GLuint program;
  std::vector<EffectParameter> params;

  void GetParameters();
};

#endif // __EFFECT__
