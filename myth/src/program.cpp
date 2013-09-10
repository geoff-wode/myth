#include <SDL.h>
#include <program.h>

Program::Program(const char* const windowTitle)
  : hasQuit(false), windowTitle(windowTitle)
{
}

Program::~Program()
{
}

//-----------------------------------------------------------

void Program::Run()
{
  Initialise();
  LoadContent();

  unsigned int prevTime = 0;
  while (!hasQuit)
  {
    EventLoop();
    if (!hasQuit)
    {
      unsigned int now = SDL_GetTicks();
      if (prevTime == 0) { prevTime = now; }
      const float elapsedMS = (float)(now - prevTime);
      prevTime = now;

      Update(elapsedMS);
      Render(elapsedMS);
    }
  }
}

//-----------------------------------------------------------

void Program::Quit()
{
  hasQuit = true;
}

//-----------------------------------------------------------

void Program::EventLoop()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    case SDL_QUIT: Quit(); break;
    default: break;
    }
  }
}

//-----------------------------------------------------------
void Program::Initialise()
{
}

//-----------------------------------------------------------
void Program::LoadContent()
{
}

//-----------------------------------------------------------
void Program::Update(float elapsedMS)
{
}

//-----------------------------------------------------------
void Program::Render(float elapsedMS)
{
  Device.GetWindow()->SwapBuffers();
}
