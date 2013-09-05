#include <debug.h>
#include <SOIL.h>
#include <texture.h>
#include <renderstate.h>
#include <boost/scoped_ptr.hpp>
#include <gl_loader/gl_loader.h>

//--------------------------------------------------

Texture2D::Texture2D(const std::string& name)
  : name(name),
    width(0), height(0),
    wrapModeS(GL_REPEAT), wrapModeT(GL_REPEAT)
{
  glGenTextures(1, &handle);
}

Texture2D::~Texture2D()
{
  glDeleteTextures(1, &handle);
}

//--------------------------------------------------

bool Texture2D::Load()
{
  // Load the image data from file...
  int channels;
  unsigned char* img = SOIL_load_image(name.c_str(), (int*)&width, (int*)&height, &channels, SOIL_LOAD_AUTO);
  LOG("loaded img: %s - #channels = %d, size = %dx%d\n", name.c_str(), channels, width, height);

  handle = SOIL_create_OGL_texture(
    img, width, height, channels, handle,
    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);

  SOIL_free_image_data(img);

  Activate();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModeS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModeT);
  Deactivate();

  return true;
}

//--------------------------------------------------
void Texture2D::Activate() const
{
  glBindTexture(GL_TEXTURE_2D, handle);
}

void Texture2D::Deactivate()
{
  glBindTexture(GL_TEXTURE_2D, 0);
}

//--------------------------------------------------

void Texture2D::SetWrapModes(GLenum s, GLenum t)
{
  if ( (wrapModeS != s) || (wrapModeT != t) )
  {
    Activate();

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
    
    Deactivate();
  }
}

void Texture2D::SetWrapS(GLenum value)
{
  if (wrapModeS != value)
  {
    Activate();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, value);
    wrapModeS = value;
    Deactivate();
  }
}

void Texture2D::SetWrapT(GLenum value)
{
  if (wrapModeT != value)
  {
    Activate();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, value);
    wrapModeT = value;
    Deactivate();
  }
}
