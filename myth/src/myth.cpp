#include <myth.h>
#include <effect.h>
#include <vertexdeclaration.h>
#include <boost/make_shared.hpp>

//-----------------------------------------------------------

Myth::Myth(const char* const windowTitle)
  : Program(windowTitle)
{
}

Myth::~Myth()
{
}

//-----------------------------------------------------------
void Myth::Initialise()
{
  Device.Initialise(false, 1280, 720);

  Program::Initialise();
}

//-----------------------------------------------------------
void Myth::LoadContent()
{
  simplestEffect = boost::make_shared<Effect>();
  simplestEffect->Load("assets\\shaders\\simplest");

  fullScreenQuad = boost::make_shared<FullScreenQuad>();
  fullScreenQuad->Load();

  Program::LoadContent();
}

//-----------------------------------------------------------
void Myth::Update(float elapsedMS)
{
  Program::Update(elapsedMS);
}

//-----------------------------------------------------------
void Myth::Render(float elapsedMS)
{
  Device.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, clearState);

  simplestEffect->Use();
  fullScreenQuad->Draw(&Device);
  
  Program::Render(elapsedMS);
}
