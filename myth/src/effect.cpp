#include <effect.h>
#include <dirio.h>
#include <debug.h>
#include <vector>
#include <memory>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

//--------------------------------------------------

static const char CommonShaderCode[] =
{
	"#version 330\n"
	"\n"
	"#define PI 3.14159\n"
	"#define TWO_PI (PI * 2)\n"
	"#define PI_OVER_2 (PI * 0.5f)\n"
	"\n"
	"layout (std140) uniform GlobalUniforms\n"
	"{\n"
  " mat4 WorldMatrix;                // Model- to view-space transform.\n"
  " mat4 ViewMatrix;                 // View- to projection-space transform.\n"
  " mat4 ProjectionMatrix;           // Projection- to screen-space transform.\n"
  " mat4 NormalMatrix;               // Surface normal transform.\n"
  " mat4 WorldViewProjectionMatrix;  // The full transformative beans.\n"
	"};\n"
};

static const char* const AttributeNames[] =
{
  "Position",
  "Normal",
  "TextureCoord0",
  "TextureCoord1",
  "TextureCoord2",
  "TextureCoord3"
};
static const size_t AttributeNamesCount = sizeof(AttributeNames) / sizeof(AttributeNames[0]);

//--------------------------------------------------

static Effect* currentEffect;

//--------------------------------------------------

static std::vector<GLuint> Compile(const std::vector<std::string>& shaderNames);
static GLuint CompileShader(const char* const code, int codeLength, GLenum type);
static bool LinkProgram(const std::vector<GLuint> shaders, GLuint program);

static void QueryShaderAttributes(const GLuint program);

static void QueryUniformBlockMembers(const GLuint program, const GLuint blockIndex);

//--------------------------------------------------

Effect::Effect()
  : program(glCreateProgram())
{
}

Effect::~Effect()
{
  glDeleteProgram(program);
}

//--------------------------------------------------

bool Effect::Load(const char* const name)
{
  bool loaded = false;

  std::vector<std::string> shaderNames;
  ListFiles(name, shaderNames);
  std::vector<GLuint> shaders = Compile(shaderNames);
	LOG("loading shader: %s", name);
  if (LinkProgram(shaders, program))
  {
    LOG(" - built ok\n");
    QueryShaderAttributes(program);
    GetParameters();
    loaded = true;
  }
  return loaded;
}

//--------------------------------------------------

void Effect::Use()
{
  if (currentEffect != this)
  {
    currentEffect = this;
    glUseProgram(program);
  }

  for (size_t i = 0; i < params.size(); ++i)
  {
    if (params[i].tainted)
    {
		  switch (params[i].type)
		  {
      case GL_INT:         glUniform1i(params[i].location, *(GLint*)params[i].cache); break;
      case GL_UNSIGNED_INT:glUniform1ui(params[i].location, *(GLuint*)params[i].cache); break;
		  case GL_FLOAT:		  glUniform1f(params[i].location, *(float*)params[i].cache); break;
		  case GL_FLOAT_VEC2: glUniform2fv(params[i].location, 1, (float*)params[i].cache); break;
		  case GL_FLOAT_VEC3: glUniform3fv(params[i].location, 1, (float*)params[i].cache); break;
		  case GL_FLOAT_VEC4: glUniform4fv(params[i].location, 1, (float*)params[i].cache); break;
		  case GL_FLOAT_MAT2: glUniformMatrix2fv(params[i].location, 1, GL_FALSE, (float*)params[i].cache); break;
		  case GL_FLOAT_MAT3: glUniformMatrix3fv(params[i].location, 1, GL_FALSE, (float*)params[i].cache); break;
		  case GL_FLOAT_MAT4: glUniformMatrix4fv(params[i].location, 1, GL_FALSE, (float*)params[i].cache); break;
		  }
      params[i].tainted = false;
    }
  }
}

//--------------------------------------------------

GLint Effect::GetAttributeIndex(const char* const name)
{
  for (GLint i = 0; i < AttributeNamesCount; ++i)
  {
    if (0 == std::strcmp(AttributeNames[i], name)) { return i; }
  }
  LOG("unknown shader attribute requested: %s\n", name);
  return -1;
}

//--------------------------------------------------

static std::vector<GLuint> Compile(const std::vector<std::string>& shaderNames)
{
  std::vector<char> fileContent;
  std::vector<GLuint> shaders(shaderNames.size());
  for (size_t i = 0; i < shaderNames.size(); ++i)
  {
    LoadFile(shaderNames[i], fileContent);
    GLenum type = 0;
    if (std::string::npos != shaderNames[i].find("vs.glsl")) { type = GL_VERTEX_SHADER; }
    else if (std::string::npos != shaderNames[i].find("fs.glsl")) { type = GL_FRAGMENT_SHADER; }
    else if (std::string::npos != shaderNames[i].find("gs.glsl")) { type = GL_GEOMETRY_SHADER; }

    if (type)
    {
      shaders[i] = CompileShader(fileContent.data(), fileContent.size(), type);
    }
  }
  return shaders;
}

//--------------------------------------------------

static GLuint CompileShader(const char* const code, int codeLength, GLenum type)
{
  static const char AttribLayout[] =
  {
	  "\n"
    "layout (location = 0) in vec3 Position;\n"
    "layout (location = 1) in vec3 Normal;\n"
    "layout (location = 2) in vec2 TextureCoord0;\n"
    "layout (location = 3) in vec2 TextureCoord1;\n"
    "layout (location = 4) in vec2 TextureCoord2;\n"
    "layout (location = 5) in vec2 TextureCoord3;\n"
	  "\n"
  };

  std::vector<const char*> source;
  std::vector<GLint> lengths;

  source.push_back(CommonShaderCode);
  lengths.push_back(sizeof(CommonShaderCode));

  if (GL_VERTEX_SHADER == type)
  {
    source.push_back(AttribLayout);
    lengths.push_back(sizeof(AttribLayout));
  }

  source.push_back(code);
  lengths.push_back(codeLength);

	const GLuint shader = glCreateShader(type);
	glShaderSource(shader, source.size(), source.data(), lengths.data());
	glCompileShader(shader);

	GLint compiledOK;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiledOK);

	if (!compiledOK)
	{
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> log(logLength+1);
		glGetShaderInfoLog(shader, logLength, NULL, log.data());
		LOG("%s\n", log.data());
	}

	return shader;
}

//--------------------------------------------------

static bool LinkProgram(const std::vector<GLuint> shaders, GLuint program)
{
	for (size_t i = 0; i < shaders.size(); ++i) { glAttachShader(program, shaders[i]); }

	glLinkProgram(program);

	for (size_t i = 0; i < shaders.size(); ++i)
	{
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}

	GLint linkedOK;
	glGetProgramiv(program, GL_LINK_STATUS, &linkedOK);
	if (GL_TRUE != linkedOK)
	{
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> log(logLength+1);
		glGetProgramInfoLog(program, logLength, NULL, log.data());
		LOG("\n%s\n", log.data());
	}
  return (GL_TRUE == linkedOK);
}

//--------------------------------------------------
static void QueryShaderAttributes(const GLuint program)
{
	GLint numAttributes;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numAttributes);
  LOG("  has %d attributes\n", numAttributes);

	for (GLint i = 0; i < numAttributes; ++i)
	{
		char attrName[128];
		GLint numUnits;
		GLenum type;
		glGetActiveAttrib(program, i, sizeof(attrName)-1, NULL, &numUnits, &type, attrName);
		LOG("    %s\n", attrName);
	}
}

//--------------------------------------------------
void Effect::GetParameters()
{
	GLint numUniforms;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);
  LOG("  has %d uniforms\n", numUniforms);

	std::vector<GLuint> indices(numUniforms);
	for (GLint i = 0; i < numUniforms; ++i) { indices[i] = i; }

  // Get the types of all uniforms...
	std::vector<GLint> types(numUniforms);
	glGetActiveUniformsiv(program, numUniforms, indices.data(), GL_UNIFORM_TYPE, types.data());

  // Get the block indices of all uniforms...
	std::vector<GLint> blockIndices(numUniforms);
	glGetActiveUniformsiv(program, numUniforms, indices.data(), GL_UNIFORM_BLOCK_INDEX, blockIndices.data());

	for (int i = 0; i < numUniforms; ++i)
	{
    EffectParameter p;

    glGetActiveUniformName(program, i, sizeof(p.name), NULL, p.name);

		if (-1 == blockIndices[i])
		{
  		// parameter is 
      EffectParameter p;
      p.type = types[i];
      p.location = glGetUniformLocation(program, p.name);
			params.push_back(p);

      LOG("    %s\n", p.name);
		}
	}

	const GLuint uniformBlockIndex = glGetUniformBlockIndex(program, "GlobalUniforms");
	if (GL_INVALID_INDEX != uniformBlockIndex)
  {
		glUniformBlockBinding(program, uniformBlockIndex, 0);
    QueryUniformBlockMembers(program, uniformBlockIndex);
  }
}

//--------------------------------------------------
static void QueryUniformBlockMembers(const GLuint program, const GLuint blockIndex)
{
	GLint numBlockUniforms;
	glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numBlockUniforms);

	LOG("  block 'GlobalUniforms' @ idx %d\n", blockIndex);

	std::vector<GLuint> blockVarIndices(numBlockUniforms);
	glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (GLint*)blockVarIndices.data());

	std::vector<GLint> types(numBlockUniforms);
	std::vector<GLint> offsets(numBlockUniforms);
	glGetActiveUniformsiv(program, numBlockUniforms, blockVarIndices.data(), GL_UNIFORM_TYPE, types.data());
	glGetActiveUniformsiv(program, numBlockUniforms, blockVarIndices.data(), GL_UNIFORM_OFFSET, offsets.data());

	for (GLint i = 0; i < numBlockUniforms; ++i)
	{
    char uniformName[128] = { 0 };
		glGetActiveUniformName(program, blockVarIndices[i], sizeof(uniformName)-1, NULL, uniformName);
		LOG("    %s is at %d\n", uniformName, offsets[i]);
	}
}
