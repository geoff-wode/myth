#if ! defined(__SAMPLER__)
#define __SAMPLER__

#include <boost/shared_ptr.hpp>
#include <gl_loader/gl_loader.h>

struct Sampler2D
{
  Sampler2D()
    : handle(0),
      minFilter(GL_LINEAR_MIPMAP_LINEAR), magFilter(GL_LINEAR),
      wrapS(GL_REPEAT),
      wrapT(GL_REPEAT),
      maxAnisotropy(1.0f)
  {
    glGenSamplers(1, (GLuint*)&handle);
    glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, magFilter);
    glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, minFilter);
    glSamplerParameteri(handle, GL_TEXTURE_WRAP_S, wrapS);
    glSamplerParameteri(handle, GL_TEXTURE_WRAP_T, wrapT);
    glSamplerParameterf(handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
  }

  Sampler2D(GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT, float maxAnisotropy = 1.0f)
    : handle(0),
      minFilter(minFilter), magFilter(magFilter),
      wrapS(wrapS),
      wrapT(wrapT),
      maxAnisotropy(maxAnisotropy)
  {
    glGenSamplers(1, (GLuint*)&handle);
    glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, magFilter);
    glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, minFilter);
    glSamplerParameteri(handle, GL_TEXTURE_WRAP_S, wrapS);
    glSamplerParameteri(handle, GL_TEXTURE_WRAP_T, wrapT);
    glSamplerParameterf(handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
  }

  ~Sampler2D() { glDeleteSamplers(1, &handle);  }

  void Bind(unsigned int textureUnit) { glBindSampler(GL_TEXTURE0 + textureUnit, handle); }
  static void Unbind(unsigned int textureUnit) { glBindSampler(GL_TEXTURE0 + textureUnit, 0); }

  const GLuint handle;
  const GLenum minFilter;
  const GLenum magFilter;
  const GLenum wrapS;
  const GLenum wrapT;
  const float maxAnisotropy;
};


#endif // __SAMPLER__
