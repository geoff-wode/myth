#include <memory>
#include <effectparameter.h>
#include <gl_loader/gl_loader.h>

EffectParameter::EffectParameter(GLuint program, GLint location, GLenum type)
  : program(program), location(location), type(type)
{
}

void EffectParameter::Set(int value)              { SetValue(&value, sizeof(value)); }
void EffectParameter::Set(unsigned int value)     { SetValue(&value, sizeof(value)); }
void EffectParameter::Set(float value)            { SetValue(&value, sizeof(value)); }
void EffectParameter::Set(const glm::vec2& value) { SetValue(&value, sizeof(value)); }
void EffectParameter::Set(const glm::vec3& value) { SetValue(&value, sizeof(value)); }
void EffectParameter::Set(const glm::vec4& value) { SetValue(&value, sizeof(value)); }
void EffectParameter::Set(const glm::mat2& value) { SetValue(&value, sizeof(value)); }
void EffectParameter::Set(const glm::mat3& value) { SetValue(&value, sizeof(value)); }
void EffectParameter::Set(const glm::mat4& value) { SetValue(&value, sizeof(value)); }

void EffectParameter::SetValue(const void* const value, size_t size)
{
  if (0 != std::memcmp(value, cache, size))
  {
    std::memcpy(cache, value, size);

		switch (type)
		{
    case GL_INT:          glProgramUniform1i(program, location, *(GLint*)cache); break;
    case GL_UNSIGNED_INT: glProgramUniform1ui(program, location, *(GLuint*)cache); break;
		case GL_FLOAT:		    glProgramUniform1f(program, location, *(float*)cache); break;

		case GL_FLOAT_VEC2:   glProgramUniform2fv(program, location, 1, (float*)cache); break;
		case GL_FLOAT_VEC3:   glProgramUniform3fv(program, location, 1, (float*)cache); break;
		case GL_FLOAT_VEC4:   glProgramUniform4fv(program, location, 1, (float*)cache); break;

		case GL_FLOAT_MAT2:   glProgramUniformMatrix2fv(program, location, 1, GL_FALSE, (float*)cache); break;
		case GL_FLOAT_MAT3:   glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, (float*)cache); break;
		case GL_FLOAT_MAT4:   glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, (float*)cache); break;
		}
  }
}

