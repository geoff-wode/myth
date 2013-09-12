#include <debug.h>
#include <SOIL.h>
#include <texture.h>
#include <renderstate.h>
#include <boost/scoped_ptr.hpp>
#include <gl_loader/gl_loader.h>

//--------------------------------------------------

Texture2D::Texture2D()
  : width(0), height(0),
    wrapModeS(GL_REPEAT), wrapModeT(GL_REPEAT)
{
  glGenTextures(1, &handle);
}

Texture2D::~Texture2D()
{
  glDeleteTextures(1, &handle);
}

//--------------------------------------------------

bool Texture2D::Load(const char* const name)
{
  // Load the image data from file...
  int channels;
  unsigned char* img = SOIL_load_image(name, (int*)&width, (int*)&height, &channels, SOIL_LOAD_AUTO);

  if (!img)
  {
    LOG("texture load failed: %s (%s)\n", name, SOIL_last_result());
    return false;
  }

  LOG("texture loaded %s: #channels = %d, size = %dx%d\n", name, channels, width, height);

  // Setting the active texture to beyond the range of the renderer's supported texture units
  // ensures that textures already bound to the render state are not disrupted when a new
  // texture is loaded (although it's rather poor form to be loading image files while inside
  // a render loop anyway...)
  // Note that this does NOT happen when binding the texture because textures can be bound to
  // the GL context for any number of reasons and not just for loading or rendering...
  glActiveTexture(GL_TEXTURE0 + RenderState::MaxTextureUnits);

  handle = SOIL_create_OGL_texture(
    img, width, height, channels, handle,
    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);

  SOIL_free_image_data(img);

  Bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModeS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModeT);
  Unbind();

  return true;
}

//--------------------------------------------------
void Texture2D::Bind() const
{
  glBindTexture(GL_TEXTURE_2D, handle);
}

void Texture2D::Unbind()
{
  glBindTexture(GL_TEXTURE_2D, 0);
}

//--------------------------------------------------

void Texture2D::SetWrapModes(GLenum s, GLenum t)
{
  if ( (wrapModeS != s) || (wrapModeT != t) )
  {
    glActiveTexture(GL_TEXTURE0 + RenderState::MaxTextureUnits);  // See comment in Load().
    Bind();

    if (wrapModeS != s)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
      wrapModeS = s;
    }

    if (wrapModeT != t)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
      wrapModeT = t;
    }
    
    Unbind();
  }
}

void Texture2D::SetWrapS(GLenum value)
{
  if (wrapModeS != value)
  {
    glActiveTexture(GL_TEXTURE0 + RenderState::MaxTextureUnits);  // See comment in Load().

    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, value);
    wrapModeS = value;
    Unbind();
  }
}

void Texture2D::SetWrapT(GLenum value)
{
  if (wrapModeT != value)
  {
    glActiveTexture(GL_TEXTURE0 + RenderState::MaxTextureUnits);  // See comment in Load().
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, value);
    wrapModeT = value;
    Unbind();
  }
}
