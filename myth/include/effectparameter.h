#if ! defined(__EFFECT_PARAMETER__)
#define __EFFECT_PARAMETER__

#include <glm/glm.hpp>
#include <gl_loader/gl_loader.h>

class EffectParameter
{
public:
  EffectParameter& operator=(int value);
  EffectParameter& operator=(unsigned int value);
  EffectParameter& operator=(float value);
  EffectParameter& operator=(const glm::vec2& value);
  EffectParameter& operator=(const glm::vec3& value);
  EffectParameter& operator=(const glm::vec4& value);
  EffectParameter& operator=(const glm::mat2& value);
  EffectParameter& operator=(const glm::mat3& value);
  EffectParameter& operator=(const glm::mat4& value);

private:
  EffectParameter();

  char name[64];
  GLenum type;
  GLint location;
  bool tainted;
  double cache[16];

  friend class Effect;
};

#endif // __EFFECT_PARAMETER__
