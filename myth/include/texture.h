#if ! defined(__TEXTURE__)
#define __TEXTURE__

#include <string>
#include <gl_loader/gl_loader.h>

class Texture2D
{
public:
  Texture2D();
  ~Texture2D();

  // Loads the image named in the constructor into GPU memory.
  bool Load(const char* const name);

  size_t GetWidth() const { return width; }
  size_t GetHeight() const { return height; }

  void SetWrapModes(GLenum s, GLenum t);
  void SetWrapS(GLenum value);
  void SetWrapT(GLenum value);

  void Bind() const;
  static void Unbind();

private:
  unsigned int handle;
  size_t width;
  size_t height;
  GLenum wrapModeS;
  GLenum wrapModeT;
};

#endif // __TEXTURE__
