#if ! defined(__EFFECT_PARAMETER__)
#define __EFFECT_PARAMETER__

#include <glm/glm.hpp>
#include <gl_loader/gl_loader.h>

class EffectParameter
{
public:
  EffectParameter(GLuint program, GLint location, GLenum type);

  void Set(int value);
  void Set(unsigned int value);
  void Set(float value);
  void Set(const glm::vec2& value);
  void Set(const glm::vec3& value);
  void Set(const glm::vec4& value);
  void Set(const glm::mat2& value);
  void Set(const glm::mat3& value);
  void Set(const glm::mat4& value);

private:
  void SetValue(const void* const value, size_t size);

  GLuint program;
  GLint location;
  GLenum type;

  double cache[16];
};

#endif // __EFFECT_PARAMETER__
