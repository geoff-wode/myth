#if ! defined(__EFFECT__)
#define __EFFECT__

#include <map>
#include <effectparameter.h>
#include <glm/glm.hpp>
#include <gl_loader/gl_loader.h>
#include <boost/shared_ptr.hpp>

class Effect
{
public:
  Effect();
  virtual ~Effect();

  virtual bool Load(const char* const name);

  void Use();

  // Return the index for attribute 'name' in the shader.
  static GLint GetAttributeIndex(const char* const name);

  EffectParameter* GetEffectParameter(const char* const name) const;

private:
  GLuint program;

  std::map<std::string, boost::shared_ptr<EffectParameter>> params;

  void GetParameters();
};

#endif // __EFFECT__
