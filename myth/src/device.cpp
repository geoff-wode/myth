#include <SDL.h>
#include <debug.h>
#include <device.h>
#include <buffers.h>
#include <light.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

//--------------------------------------------------------------------

struct GlobalUniforms
{
  glm::vec4 CameraPos;                  // World coordinate of the "camera".
  glm::mat4 WorldMatrix;                // Model- to view-space transform.
  glm::mat4 ViewMatrix;                 // View- to projection-space transform.
  glm::mat4 ProjectionMatrix;           // Projection- to screen-space transform.
  glm::mat4 NormalMatrix;               // Surface normal transform.
  glm::mat4 WorldViewProjectionMatrix;  // The full transformative beans.
};

//--------------------------------------------------------------------

static void OnExit();
static void ForceClearState(const ClearState& state);
static void ForceRenderState(const RenderState& state);
static void ApplyRenderState(const RenderState& renderState, const Device::ShaderVariables& shaderVars);
static void ApplyShader(boost::shared_ptr<Shader> value);
static void ApplyTextureUnits(const TextureUnit textureUnits[]);
static void ApplyVAO(GLuint value);
static void ApplyCulling(bool enabled, GLenum faceToCull, GLenum frontFace);
static void ApplyColourMask(const glm::bvec4& value);
static void ApplyDepthMask(bool value);
static void ApplyDepthFunc(bool enabled, GLenum func);

//--------------------------------------------------------------------

static SDL_Window* mainWindow = NULL;
static SDL_GLContext glContext = NULL;
static ClearState currentClearState;
static RenderState currentRenderState;
static GlobalUniforms globalUniforms;
static boost::shared_ptr<UniformBuffer> uniformBuffer;
static bool globalUniformsDirty = false;
static bool operator==(const Light& a, const Light& b);
static bool operator!=(const Light& a, const Light& b);

//--------------------------------------------------------------------

bool Device::Initialise(const std::string& title, int width, int height, bool fullScreen)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	atexit(OnExit);

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

  if (fullScreen)
  {
    mainWindow = SDL_CreateWindow(
      title.c_str(),
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      0, 0, // width & height are ignored in full screen mode
      SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
  }
  else
  {
    mainWindow = SDL_CreateWindow(
      title.c_str(),
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      width, height,
      SDL_WINDOW_OPENGL);
  }

  if (mainWindow)
  {
    glContext = SDL_GL_CreateContext(mainWindow);
    LOG("failed to create OpenGL context: %s\n", SDL_GetError());
  }
  else
  {
    LOG("failed to create main window: %s\n", SDL_GetError());
  }

	ogl_LoadFunctions();
  LOG("GL v%d.%d\n", ogl_GetMajorVersion(), ogl_GetMinorVersion());
  LOG("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  LOG("Vendor: %s\n", glGetString(GL_VENDOR));
  LOG("Renderer: %s\n", glGetString(GL_RENDERER));

  ForceClearState(currentClearState);
  ForceRenderState(currentRenderState);

  uniformBuffer = boost::make_shared<UniformBuffer>(sizeof(GlobalUniforms), 0);

  SDL_SetRelativeMouseMode(SDL_TRUE);

  return (NULL != mainWindow) && (NULL != glContext);
}

//--------------------------------------------------------------------

void Device::Clear(const ClearState& clearState, GLenum buffers)
{
  ApplyColourMask(clearState.colourMask);
  ApplyDepthMask(clearState.depthMask);

  if (currentClearState.colour != clearState.colour)
  {
    glClearColor(clearState.colour.r, clearState.colour.g, clearState.colour.b, clearState.colour.a);
    currentClearState.colour = clearState.colour;
  }

  if (currentClearState.depth != clearState.depth)
  {
    glClearDepth(clearState.depth);
    currentClearState.depth = clearState.depth;
  }

  glClear(buffers);
}

//--------------------------------------------------------------------

void Device::Draw(GLenum primitiveType, GLint startVertex, GLint vertexCount, const RenderState& renderState)
{
  ApplyRenderState(renderState, shaderVars);
  glDrawArrays(primitiveType, startVertex, vertexCount);
}

//--------------------------------------------------------------------

void Device::DrawIndexed(GLenum primitiveType, size_t indexCount, GLenum indexType, size_t bufferOffset, const RenderState& renderState)
{
  ApplyRenderState(renderState, shaderVars);
  glDrawElements(primitiveType, indexCount, indexType, (const void*)bufferOffset);
}

//--------------------------------------------------------------------

void Device::SwapBuffers()
{
  SDL_GL_SwapWindow(mainWindow);
}

//--------------------------------------------------------------------
const ClearState& Device::GetClearState() const
{
  return currentClearState;
}

const RenderState& Device::GetRenderState() const
{
  return currentRenderState;
}

//--------------------------------------------------------------------

static void OnExit()
{
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(mainWindow);
  SDL_Quit();
}

//--------------------------------------------------------------------
static void ForceRenderState(const RenderState& state)
{
  glColorMask(state.colourMask.r, state.colourMask.g, state.colourMask.b, state.colourMask.a);
  glDepthMask(state.depthMask);
  
  if (state.enableDepthTest)
  {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(state.depthFunc);
  }
  else
  {
    glDisable(GL_DEPTH_TEST);
  }

  if (state.enableCulling)
  {
    glEnable(GL_CULL_FACE);
    glCullFace(state.cullFace);
    glFrontFace(state.frontFace);
  }
  else
  {
    glDisable(GL_CULL_FACE);
  }

  for (size_t i = 0; i < RenderState::MaxTextureUnits; ++i)
  {
    glActiveTexture(GL_TEXTURE0 + i);
  }
}

//-----------------------------------------------------
static void ForceClearState(const ClearState& state)
{
  glClearColor(state.colour.r, state.colour.g, state.colour.b, state.colour.a);
  glClearDepth(state.depth);
}

//-----------------------------------------------------
static void ApplyVAO(GLuint value)
{
  if (currentRenderState.vao != value)
  {
    currentRenderState.vao = value;
    glBindVertexArray(currentRenderState.vao);
  }
}

//-----------------------------------------------------
static void ApplyShader(boost::shared_ptr<Shader> value)
{
  if (currentRenderState.shader != value)
  {
    currentRenderState.shader = value;
    currentRenderState.shader->Use();
  }
  currentRenderState.shader->ApplyUniforms();
}

//-----------------------------------------------------
static void ApplyTextureUnits(const TextureUnit value[])
{
  for (size_t i = 0; i < RenderState::MaxTextureUnits; ++i)
  {
    glActiveTexture(GL_TEXTURE0 + i);

    if (value[i].active)
    {
      if (value[i].texture != currentRenderState.textureUnits[i].texture)
      {
        value[i].texture->Activate();
      }
    }
    else
    {
      Texture2D::Deactivate();
    }

    currentRenderState.textureUnits[i] = value[i];
  }
}

//-----------------------------------------------------
static void ApplyCulling(bool enabled, GLenum faceToCull, GLenum frontFace)
{
  if (currentRenderState.enableCulling != enabled)
  {
    if (currentRenderState.enableCulling && !enabled)
    {
      glDisable(GL_CULL_FACE);
    }
    else
    {
      glEnable(GL_CULL_FACE);
    }
    currentRenderState.enableCulling = enabled;
  }

  if (currentRenderState.enableCulling)
  {
    if (currentRenderState.cullFace != faceToCull)
    {
      glCullFace(faceToCull);
      currentRenderState.cullFace = faceToCull;
    }

    if (currentRenderState.frontFace != frontFace)
    {
      glFrontFace(frontFace);
      currentRenderState.frontFace = frontFace;
    }
  }
}

//-----------------------------------------------------
static void ApplyColourMask(const glm::bvec4& value)
{
  if (currentRenderState.colourMask != value)
  {
    glColorMask(value.r, value.g, value.b, value.a);
    currentRenderState.colourMask = value;
  }
}

//-----------------------------------------------------
static void ApplyDepthMask(bool value)
{
  if (currentRenderState.depthMask != value)
  {
    glDepthMask(value);
    currentRenderState.depthMask = value;
  }
}

//-----------------------------------------------------
static void ApplyDepthFunc(bool enabled, GLenum func)
{
  if (currentRenderState.enableDepthTest != enabled)
  {
    if (currentRenderState.enableDepthTest && !enabled)
    {
      glDisable(GL_DEPTH_TEST);
    }
    else
    {
      glEnable(GL_DEPTH_TEST);
    }
    currentRenderState.enableDepthTest = enabled;
  }

  if (currentRenderState.enableDepthTest)
  {
    if (currentRenderState.depthFunc != func)
    {
      glDepthFunc(func);
      currentRenderState.depthFunc = func;
    }
  }
}

//--------------------------------------------------------------------

static void ApplyRenderState(const RenderState& renderState, const Device::ShaderVariables& shaderVars)
{
  // Update the shared uniform buffer...
  {
    globalUniforms.WorldMatrix = shaderVars.WorldMatrix;

    globalUniforms.CameraPos = glm::vec4(shaderVars.CameraPos, 1);
    globalUniforms.ViewMatrix = shaderVars.ViewMatrix;
    globalUniforms.ProjectionMatrix = shaderVars.ProjectionMatrix;

    // Create a matrix that rotates normals (no scaling or translation)...
    globalUniforms.NormalMatrix = glm::mat4(glm::transpose(glm::inverse(glm::mat3(globalUniforms.WorldMatrix))));

    // Create the model-to-screen space matrix...
    globalUniforms.WorldViewProjectionMatrix = globalUniforms.ProjectionMatrix * globalUniforms.ViewMatrix * globalUniforms.WorldMatrix;

    uniformBuffer->Enable();
    uniformBuffer->SetData(&globalUniforms, sizeof(globalUniforms), 0);
  }

  ApplyShader(renderState.shader);
  ApplyVAO(renderState.vao);
  ApplyTextureUnits(renderState.textureUnits);
  ApplyColourMask(renderState.colourMask);
  ApplyDepthMask(renderState.depthMask);
  ApplyDepthFunc(renderState.enableDepthTest, renderState.depthFunc);
}
