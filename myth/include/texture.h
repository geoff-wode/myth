#if ! defined(__TEXTURE__)
#define __TEXTURE__

#include <string>

class Texture2D
{
public:
  Texture2D(const std::string& name);
  ~Texture2D();

  // Loads the image named in the constructor into GPU memory.
  bool Load();

  size_t GetWidth() const { return width; }
  size_t GetHeight() const { return height; }

  void Activate() const;
  static void Deactivate();

private:
  const std::string name;
  unsigned int handle;
  size_t width;
  size_t height;
};

#endif // __TEXTURE__
