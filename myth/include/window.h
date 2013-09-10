#if ! defined(__WINDOW__)
#define __WINDOW__

#include <SDL.h>

class Window
{
public:
  Window(int width, int height, bool fullScreen = false);
  Window(const char* const title, int width, int height, bool fullScreen = false);

  ~Window();

  void SetFullScreen(bool value);
  void SetTitle(const char* const value);
  void SwapBuffers();

private:
  int width;
  int height;
  char title[65];
  SDL_Window* window;
  SDL_GLContext glContext;
};

#endif // __WINDOW__
