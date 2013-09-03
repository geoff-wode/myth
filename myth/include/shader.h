#if ! defined(__SHADER__)
#define __SHADER__

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <boost/noncopyable.hpp>
#include <gl_loader/gl_loader.h>

struct ShaderUniform;

class Shader : public boost::noncopyable
{
public:
  Shader(const std::string& name);
  ~Shader();

  void Use();
  void ApplyUniforms();

  // Return the index for attribute 'name' in the shader.
  GLint GetAttributeIndex(const std::string& name);

  void SetUniform(const std::string& name, int value);
  void SetUniform(const std::string& name, unsigned int value);
  void SetUniform(const std::string& name, float value);
  void SetUniform(const std::string& name, const glm::vec2& value);
  void SetUniform(const std::string& name, const glm::vec3& value);
  void SetUniform(const std::string& name, const glm::vec4& value);
  void SetUniform(const std::string& name, const glm::mat2& value);
  void SetUniform(const std::string& name, const glm::mat3& value);
  void SetUniform(const std::string& name, const glm::mat4& value);

private:
  const GLuint programHandle;
  std::map<std::string, ShaderUniform> uniforms;

  void SetUniform(const std::string& name, const void* const data, size_t size);
};

#endif // __SHADER__
