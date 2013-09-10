#include <window.h>

static SDL_Window* CreateWindow(const char* const title, int width, int height, bool fullScreen)
{
  const Uint32 flags(SDL_WINDOW_OPENGL | (fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));

  return SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
}

Window::Window(int width, int height, bool fullScreen)
  : window(CreateWindow("Program", width, height, fullScreen)),
    glContext(SDL_GL_CreateContext(window))
{  
}

Window::Window(const char* const title, int width, int height, bool fullScreen)
  : window(CreateWindow("Program", width, height, fullScreen)),
    glContext(SDL_GL_CreateContext(window))
{
}

Window::~Window()
{
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
}

void Window::SetFullScreen(bool value)
{
  SDL_SetWindowFullscreen(window, value ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void Window::SetTitle(const char* const value)
{
  SDL_SetWindowTitle(window, value);
}

void Window::SwapBuffers()
{
  SDL_GL_SwapWindow(window);
}
