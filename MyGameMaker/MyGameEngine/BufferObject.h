#pragma once

class BufferObject
{
	unsigned int id = 0;
	int target = 0;
public:
	unsigned int Id() const { return id; }
	int Target() const { return target; }
	void LoadData(const void* data, size_t size);
	//for animations mayne add a boolean to say in the implementation if we want static draw or dynamic draw
	void LoadIndices(const unsigned int* indices, size_t num_indices);
	void UnLoad();
	void bind() const;

	BufferObject() = default;

	BufferObject(BufferObject&& other) noexcept;
	BufferObject& operator=(BufferObject&& other) noexcept = delete;

	BufferObject(const BufferObject&) = delete;
	BufferObject& operator=(const BufferObject&) = delete;
	~BufferObject();
};

