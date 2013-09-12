#include <cstdlib>
#include <debug.h>
#include <device.h>
#include <boost/make_shared.hpp>
#include <gl_loader/gl_loader.h>

//-----------------------------------------------------------

struct GlobalUniforms
{
  glm::mat4 WorldMatrix;                // Model- to view-space transform.
  glm::mat4 ViewMatrix;                 // View- to projection-space transform.
  glm::mat4 ProjectionMatrix;           // Projection- to screen-space transform.
  glm::mat4 NormalMatrix;               // Surface normal transform.
  glm::mat4 WorldViewProjectionMatrix;  // The full transformative beans.
};

//-----------------------------------------------------------

static void InitialiseSDL();
static void ForceClearState(const ClearState& state);
static void ApplyClearColour(const glm::vec4& newValue, glm::vec4& oldValue);
static void ApplyClearDepth(const float newValue, float& oldValue);

static void ForceRenderState(const RenderState& state);
static void ApplyRenderState(const RenderState& newValue, RenderState& oldValue);
static void ApplyGlobalUniforms(Device* const, boost::shared_ptr<UniformBuffer> buffer);
static void ApplyColourMask(const glm::bvec4& newValue, glm::bvec4& oldValue);
static void ApplyDepthMask(const bool newValue, bool& oldValue);
static void ApplyDepthTest(const DepthTest& newValue, DepthTest& oldValue);
static void ApplyCullTest(const CullTest& newValue, CullTest& oldValue);
static void ApplyVAO(const boost::shared_ptr<VertexArray> newValue, boost::shared_ptr<VertexArray>& oldValue);
static void ApplyTextures(const TextureUnit* const newValue, TextureUnit* const oldValue);
static void ApplyBlending(const Blending& newValue, Blending& oldValue);

//-----------------------------------------------------------

Device::Device()
{
}

Device::~Device()
{
  SDL_Quit();
}

//-----------------------------------------------------------

void Device::Initialise(bool fullScreen, int backbufferWidth, int backbufferHeight)
{
  SDL_Init(SDL_INIT_EVERYTHING);

  InitialiseSDL();
  mainWindow = boost::make_shared<Window>(backbufferWidth, backbufferHeight, fullScreen);

	ogl_LoadFunctions();
  LOG("GL version: %d.%d\n", ogl_GetMajorVersion(), ogl_GetMinorVersion());
  LOG("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  LOG("Vendor: %s\n", glGetString(GL_VENDOR));
  LOG("Renderer: %s\n", glGetString(GL_RENDERER));

  ForceClearState(clearState);

  globalUniforms = boost::make_shared<UniformBuffer>(sizeof(GlobalUniforms), 0);
}

//-----------------------------------------------------------
void Device::Clear(GLenum buffers, const ClearState& clearState)
{
  ApplyClearColour(clearState.colour, this->clearState.colour);
  ApplyClearDepth(clearState.depth, this->clearState.depth);
  ApplyColourMask(clearState.colourMask, this->clearState.colourMask);
  ApplyDepthMask(clearState.depthMask, this->clearState.depthMask);

  glClear(buffers);
}

//-----------------------------------------------------------
void Device::Draw(GLenum primitiveType, size_t vertexCount, size_t startVertex, const RenderState& renderState)
{
  ApplyGlobalUniforms(this, globalUniforms);
  ApplyRenderState(renderState, this->renderState);

  glDrawArrays(primitiveType, startVertex, vertexCount);
}

//-----------------------------------------------------------
void Device::DrawIndexed(GLenum primitiveType, size_t indexCount, size_t startIndex, GLenum indexType, const RenderState& renderState)
{
  ApplyGlobalUniforms(this, globalUniforms);
  ApplyRenderState(renderState, this->renderState);

  glDrawElements(primitiveType, indexCount, indexType, (const void*)startIndex);
}

//-----------------------------------------------------------
static void InitialiseSDL()
{
  static const struct SDLAttribute
  {
    SDL_GLattr  attr;
    int         value;
  } sdlAttributes[] =
  {
    { SDL_GL_DOUBLEBUFFER,       1 },
    { SDL_GL_RED_SIZE,           8 },
    { SDL_GL_GREEN_SIZE,         8 },
    { SDL_GL_BLUE_SIZE,          8 },
    { SDL_GL_ALPHA_SIZE,         8 },
    { SDL_GL_BUFFER_SIZE,        24 },
    { SDL_GL_MULTISAMPLEBUFFERS, 1 },
    { SDL_GL_MULTISAMPLESAMPLES, 4 }
  };
  const int NumAttributes = sizeof(sdlAttributes)/sizeof(sdlAttributes[0]);
  for (int i = 0; i < NumAttributes; ++i)
  {
    SDL_GL_SetAttribute(sdlAttributes[i].attr, sdlAttributes[i].value);
  }
}

//-----------------------------------------------------------
static void ForceClearState(const ClearState& state)
{
  glClearColor(state.colour.r, state.colour.g, state.colour.b, state.colour.a);
  glClearDepth(state.depth);
}

//-----------------------------------------------------------
static void ApplyRenderState(const RenderState& newValue, RenderState& oldValue)
{
  ApplyColourMask(newValue.colourMask, oldValue.colourMask);
  ApplyDepthMask(newValue.depthMask, oldValue.depthMask);
  ApplyDepthTest(newValue.depthTest, oldValue.depthTest);
  ApplyCullTest(newValue.cullTest, oldValue.cullTest);
  ApplyVAO(newValue.vertexArray, oldValue.vertexArray);
  ApplyTextures(newValue.textureUnits, oldValue.textureUnits);
  ApplyBlending(newValue.blending, oldValue.blending);
}

//-----------------------------------------------------------
static void ApplyGlobalUniforms(Device* const device, boost::shared_ptr<UniformBuffer> buffer)
{
  //GlobalUniforms values;
  //values.WorldMatrix = device->WorldMatrix;
  //values.ViewMatrix = device->ViewMatrix;
  //values.ProjectionMatrix = device->ProjectionMatrix;
  //values.WorldViewProjectionMatrix = values.ProjectionMatrix * values.ViewMatrix * values.WorldMatrix;
  //values.NormalMatrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(values.WorldMatrix))));
  //buffer->Bind();
  //buffer->SetData(&values, sizeof(values), 0);
  //buffer->Unbind();
}

//-----------------------------------------------------------
static void ForceRenderState(const RenderState& state)
{
  glColorMask(state.colourMask.r, state.colourMask.g, state.colourMask.b, state.colourMask.a);
  glDepthMask(state.depthMask);

  state.cullTest.enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
  glCullFace(state.cullTest.cullFace);
  glFrontFace(state.cullTest.frontFace);

  state.depthTest.enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
  glDepthFunc(state.depthTest.function);

  state.blending.enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
  glBlendFunc(state.blending.source, state.blending.destination);
}

//-----------------------------------------------------------
static void ApplyColourMask(const glm::bvec4& newValue, glm::bvec4& oldValue)
{
  if (oldValue != newValue)
  {
    glColorMask(newValue.r, newValue.g, newValue.b, newValue.a);
    oldValue = newValue;
  }
}

//-----------------------------------------------------------
static void ApplyDepthMask(const bool newValue, bool& oldValue)
{
  if (oldValue != newValue)
  {
    glDepthMask(newValue);
    oldValue = newValue;
  }
}

//-----------------------------------------------------------
static void ApplyClearColour(const glm::vec4& newValue, glm::vec4& oldValue)
{
  if (oldValue != newValue)
  {
    glClearColor(newValue.r, newValue.g, newValue.b, newValue.a);
    oldValue = newValue;
  }
}

//-----------------------------------------------------------
static void ApplyClearDepth(const float newValue, float& oldValue)
{
  if (oldValue != newValue)
  {
    glClearDepth(newValue);
    oldValue = newValue;
  }
}

//-----------------------------------------------------------
static void ApplyDepthTest(const DepthTest& newValue, DepthTest& oldValue)
{
  if (newValue.enabled)
  {
    if (!oldValue.enabled)
    {
      glEnable(GL_DEPTH_TEST);
      oldValue.enabled = true;
    }
  }
  else
  {
    if (oldValue.enabled)
    {
      glDisable(GL_DEPTH_TEST);
      oldValue.enabled = false;
    }
  }

  if (oldValue.enabled)
  {
    if (newValue.function != oldValue.function)
    {
      glDepthFunc(newValue.function);
      oldValue.function = newValue.function;
    }
  }
}

//-----------------------------------------------------------
static void ApplyCullTest(const CullTest& newValue, CullTest& oldValue)
{
  if (newValue.enabled)
  {
    if (!oldValue.enabled)
    {
      glEnable(GL_CULL_FACE);
      oldValue.enabled = true;
    }
  }
  else
  {
    if (oldValue.enabled)
    {
      glDisable(GL_CULL_FACE);
      oldValue.enabled = false;
    }
  }

  if (oldValue.enabled)
  {
    if (newValue.cullFace != oldValue.cullFace)
    {
      glCullFace(newValue.cullFace);
      oldValue.cullFace = newValue.cullFace;
    }
    if (newValue.frontFace != oldValue.frontFace)
    {
      glFrontFace(newValue.frontFace);
      oldValue.frontFace = newValue.frontFace;
    }
  }
}

//-----------------------------------------------------------
static void ApplyVAO(const boost::shared_ptr<VertexArray> newValue, boost::shared_ptr<VertexArray>& oldValue)
{
  if (newValue != oldValue)
  {
    if (newValue)
    {
      newValue->Bind();
    }
    else
    {
      if (oldValue)
      {
        oldValue->Unbind();
      }
    }
    oldValue = newValue;
  }
}

//-----------------------------------------------------------
static void ApplyTextures(const TextureUnit* const newValue, TextureUnit* const oldValue)
{
  for (size_t i = 0; i < RenderState::MaxTextureUnits; ++i)
  {
    glActiveTexture(GL_TEXTURE0 + i);

    if (oldValue[i].active && !newValue[i].active)
    {
      // The new state is disabling this texture unit...
      oldValue[i].active = false;
      oldValue[i].texture->Unbind();
    }
    else
    {
      if (newValue[i].active || (oldValue[i].texture != newValue[i].texture))
      {
        if (newValue[i].active && newValue[i].texture)
        {
          // The new state is either enabling this texture unit OR changing the texture
          // which is bound to that unit. In either case, the texture needs to be bound...
          newValue[i].texture->Bind();
          oldValue[i].active = true;
          oldValue[i].texture = newValue[i].texture;
        }
        else
        {
          ASSERTM(false, "texture unit %d activated without associated texture\n", i);
        }
      }
    }
  }

  // See the comment in Texture.cpp::Load() for why this is done:
  glActiveTexture(GL_TEXTURE0 + RenderState::MaxTextureUnits);
}

//-----------------------------------------------------------
static void ApplyBlending(const Blending& newValue, Blending& oldValue)
{
  if (newValue.enabled && !oldValue.enabled)
  {
    glEnable(GL_BLEND);
    oldValue.enabled = true;
  }
  else if (oldValue.enabled && !newValue.enabled)
  {
    glDisable(GL_BLEND);
    oldValue.enabled = false;
  }

  if (oldValue.enabled)
  {
    if ((newValue.source != oldValue.source) || (newValue.destination != oldValue.destination))
    {
      glBlendFunc(newValue.source, newValue.destination);
      oldValue.source = newValue.source;
      oldValue.destination = newValue.destination;
    }
  }
}
