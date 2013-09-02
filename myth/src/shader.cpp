#include <shader.h>
#include <files.h>
#include <debug.h>
#include <vector>
#include <memory>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

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
  " vec4 CameraPosition;             // World coordinate of the camera.\n"
  " mat4 WorldMatrix;                // Model- to view-space transform.\n"
  " mat4 ViewMatrix;                 // View- to projection-space transform.\n"
  " mat4 ProjectionMatrix;           // Projection- to screen-space transform.\n"
  " mat4 NormalMatrix;               // Surface normal transform.\n"
  " mat4 WorldViewProjectionMatrix;  // The full transformative beans.\n"
	"};\n"
	"\n"
};

//--------------------------------------------------

struct ShaderUniform
{
  ShaderUniform() : modified(false) { }
  GLenum type;
  GLuint location;
  bool modified;
  double cache[16];
};
typedef std::map<std::string, ShaderUniform> ShaderUniformMap;

//--------------------------------------------------

static std::vector<GLuint> Compile(const std::vector<std::string>& shaderNames);
static GLuint CompileShader(const char* const code, int codeLength, GLenum type);
static bool LinkProgram(const std::vector<GLuint> shaders, GLuint programHandle);
static void QueryShaderAttributes(GLuint program);
static void QueryShaderUniforms(GLuint program, ShaderUniformMap& uniforms);

//--------------------------------------------------

Shader::Shader(const std::string& name)
  : programHandle(glCreateProgram())
{
  std::vector<std::string> shaderNames;
  ListFiles(name, shaderNames);
  std::vector<GLuint> shaders = Compile(shaderNames);
	LOG("loading shader: %s", name.c_str());
  if (LinkProgram(shaders, programHandle))
  {
    LOG(" - built ok\n");
    QueryShaderAttributes(programHandle);
    QueryShaderUniforms(programHandle, uniforms);
  }
  else
  {
    exit(EXIT_FAILURE);
  }
}

Shader::~Shader()
{
  glDeleteProgram(programHandle);
}

//--------------------------------------------------

void Shader::Use()
{
  glUseProgram(programHandle);
}

//--------------------------------------------------

void Shader::ApplyUniforms()
{
  for (ShaderUniformMap::iterator it = uniforms.begin(); it != uniforms.end(); ++it)
  {
    if (it->second.modified)
    {
			switch (it->second.type)
			{
      case GL_INT:         glUniform1i(it->second.location, *(GLint*)it->second.cache); break;
      case GL_UNSIGNED_INT:glUniform1ui(it->second.location, *(GLuint*)it->second.cache); break;
			case GL_FLOAT:		  glUniform1f(it->second.location, *(float*)it->second.cache); break;
			case GL_FLOAT_VEC2: glUniform2fv(it->second.location, 1, (float*)it->second.cache); break;
			case GL_FLOAT_VEC3: glUniform3fv(it->second.location, 1, (float*)it->second.cache); break;
			case GL_FLOAT_VEC4: glUniform4fv(it->second.location, 1, (float*)it->second.cache); break;
			case GL_FLOAT_MAT2: glUniformMatrix2fv(it->second.location, 1, GL_FALSE, (float*)it->second.cache); break;
			case GL_FLOAT_MAT3: glUniformMatrix3fv(it->second.location, 1, GL_FALSE, (float*)it->second.cache); break;
			case GL_FLOAT_MAT4: glUniformMatrix4fv(it->second.location, 1, GL_FALSE, (float*)it->second.cache); break;
			}
      it->second.modified = false;
    }
  }
}

//--------------------------------------------------

GLint Shader::GetAttributeIndex(const std::string& name)
{
  return glGetAttribLocation(programHandle, name.c_str());
}

//--------------------------------------------------

void Shader::SetUniform(const std::string& name, const void* const data, size_t size)
{
  ShaderUniformMap::iterator it = uniforms.find(name);
  if (uniforms.end() != it)
  {
    if (0 != std::memcmp(it->second.cache, data, size))
    {
      std::memcpy(it->second.cache, data, size);
      it->second.modified = true;
    }
  }
}

void Shader::SetUniform(const std::string& name, int value) { SetUniform(name, &value, sizeof(value)); }
void Shader::SetUniform(const std::string& name, unsigned int value) { SetUniform(name, &value, sizeof(value)); }
void Shader::SetUniform(const std::string& name, float value) { SetUniform(name, &value, sizeof(value)); }
void Shader::SetUniform(const std::string& name, const glm::vec2& value) { SetUniform(name, &value, sizeof(value)); }
void Shader::SetUniform(const std::string& name, const glm::vec3& value) { SetUniform(name, &value, sizeof(value)); }
void Shader::SetUniform(const std::string& name, const glm::vec4& value) { SetUniform(name, &value, sizeof(value)); }
void Shader::SetUniform(const std::string& name, const glm::mat2& value) { SetUniform(name, &value, sizeof(value)); }
void Shader::SetUniform(const std::string& name, const glm::mat3& value) { SetUniform(name, &value, sizeof(value)); }
void Shader::SetUniform(const std::string& name, const glm::mat4& value) { SetUniform(name, &value, sizeof(value)); }

//--------------------------------------------------

static std::vector<GLuint> Compile(const std::vector<std::string>& shaderNames)
{
  std::vector<GLuint> shaders(shaderNames.size());

  std::vector<char> srcCode;
  for (size_t i = 0; i < shaderNames.size(); ++i)
  {
    LoadFile(shaderNames[i], srcCode);
    if (std::string::npos != shaderNames[i].find("vs.glsl"))
    {
      shaders[i] = CompileShader(srcCode.data(), srcCode.size(), GL_VERTEX_SHADER);
    }
    else if (std::string::npos != shaderNames[i].find("fs.glsl"))
    {
      shaders[i] = CompileShader(srcCode.data(), srcCode.size(), GL_FRAGMENT_SHADER);
    }
    else if (std::string::npos != shaderNames[i].find("gs.glsl"))
    {
      shaders[i] = CompileShader(srcCode.data(), srcCode.size(), GL_GEOMETRY_SHADER);
    }
  }
  return shaders;
}

//--------------------------------------------------

static GLuint CompileShader(const char* const code, int codeLength, GLenum type)
{
  const char* source[] = { CommonShaderCode, code };
  const GLint lengths[] = { sizeof(CommonShaderCode), codeLength };
	const GLuint shader = glCreateShader(type);
	glShaderSource(shader, 2, source, lengths);
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

static bool LinkProgram(const std::vector<GLuint> shaders, GLuint programHandle)
{
	for (size_t i = 0; i < shaders.size(); ++i) { glAttachShader(programHandle, shaders[i]); }

	glLinkProgram(programHandle);

	for (size_t i = 0; i < shaders.size(); ++i)
	{
		glDetachShader(programHandle, shaders[i]);
		glDeleteShader(shaders[i]);
	}

	GLint linkedOK;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &linkedOK);
	if (GL_TRUE != linkedOK)
	{
		GLint logLength;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> log(logLength+1);
		glGetProgramInfoLog(programHandle, logLength, NULL, log.data());
		LOG("\n%s\n", log.data());
	}
  return (GL_TRUE == linkedOK);
}

//--------------------------------------------------
static void QueryShaderAttributes(GLuint program)
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
static void QueryShaderUniforms(GLuint program, ShaderUniformMap& uniforms)
{
	GLint numUniforms;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);
  LOG("  has %d uniforms\n", numUniforms);

	std::vector<GLuint> indices(numUniforms);
	for (GLint i = 0; i < numUniforms; ++i) { indices[i] = i; }

	std::vector<GLint> types(numUniforms);
	glGetActiveUniformsiv(program, numUniforms, indices.data(), GL_UNIFORM_TYPE, types.data());

	std::vector<GLint> blockIndices(numUniforms);
	glGetActiveUniformsiv(program, numUniforms, indices.data(), GL_UNIFORM_BLOCK_INDEX, blockIndices.data());

	char uniformName[256] = { 0 };

	for (int i = 0; i < numUniforms; ++i)
	{
		// not a named uniform block...
		glGetActiveUniformName(program, i, sizeof(uniformName)-1, NULL, uniformName);

		if (-1 == blockIndices[i])
		{
      ShaderUniform u;
      u.type = types[i];
      u.location = glGetUniformLocation(program, uniformName);
			uniforms[uniformName] = u;
      LOG("    %s\n", uniformName);
		}
    else
    {
      char blockName[128] = { 0 };
      glGetActiveUniformBlockName(program, blockIndices[i], sizeof(blockName)-1, NULL, blockName);
      LOG("    %s::%s\n", blockName, uniformName);
    }
	}

	const GLuint uniformBlockIndex = glGetUniformBlockIndex(program, "GlobalUniforms");
	if (GL_INVALID_INDEX != uniformBlockIndex)
  {
		glUniformBlockBinding(program, uniformBlockIndex, 0);
  }
}
