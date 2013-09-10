#if ! defined(__BUFFERS__)
#define __BUFFERS__

#include <gl_loader/gl_loader.h>

class Buffer
{
public:
	const GLenum type;
	const size_t size;
	const GLenum usage;

	virtual ~Buffer()
	{
		glDeleteBuffers(1, &handle);
	}

	void Enable() { glBindBuffer(type, handle); }

	void SetData(const void* const data, size_t size, size_t offset)
	{
		glBufferSubData(type, offset, size, data);
	}

protected:
	Buffer(GLenum type, size_t size, GLenum usage)
		: type(type), size(size), usage(usage)
	{
		glGenBuffers(1, &handle);
		glBindBuffer(type, handle);
		glBufferData(type, size, NULL, usage);
    glBindBuffer(type, 0);
	}

	GLuint handle;
};

//--------------------------------------------------------------------------------------------
class VertexBuffer : public Buffer
{
public:
	VertexBuffer(size_t size, GLenum usage)
		: Buffer(GL_ARRAY_BUFFER, size, usage)
	{
		Disable();
	}

	static void Disable() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
};

//--------------------------------------------------------------------------------------------
class IndexBuffer : public Buffer
{
public:
	IndexBuffer(size_t size, GLenum usage, GLenum type)
		: Buffer(GL_ELEMENT_ARRAY_BUFFER, size, usage),
			indexType(type)
	{
		Disable();
	}

	static void Disable() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
	
	const GLenum indexType;
};

//--------------------------------------------------------------------------------------------
class UniformBuffer : public Buffer
{
public:
	UniformBuffer(size_t size, GLuint bindingPoint)
		: Buffer(GL_UNIFORM_BUFFER, size, GL_DYNAMIC_DRAW)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, handle);
		Disable();
	}

	static void Disable() { glBindBuffer(GL_UNIFORM_BUFFER, 0); }
};

#endif //__BUFFERS__
