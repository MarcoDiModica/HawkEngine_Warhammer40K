#include "BufferObject.h"
#include <GL/glew.h>

BufferObject::BufferObject(BufferObject&& other) noexcept
	: id(other.id), target(other.target)
{
	other.id = 0;
	other.target = 0;
}

void BufferObject::UnLoad()
{
	if (id != 0)
	{
		glDeleteBuffers(1, &id);
	}
		id = 0;
	
}

BufferObject::~BufferObject()
{
	UnLoad();
}

void BufferObject::bind() const
{
	glBindBuffer(target, id);
}

void BufferObject::LoadData(const void* data, size_t size)
{
	target = GL_ARRAY_BUFFER;

	if (id == 0) { 
		glGenBuffers(1, &id);
	}
	
	glBindBuffer(target, id);
	//for animation we will use GL_DYNAMIC_DRAW
	glBufferData(target, size, data, GL_STATIC_DRAW);
}

void BufferObject::LoadIndices(const unsigned int* indices, size_t num_indices)
{
	target = GL_ELEMENT_ARRAY_BUFFER;

	if (id == 0)
	{
		glGenBuffers(1, &id);
	}

	glBindBuffer(target, id);
	glBufferData(target, num_indices * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}
