#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

struct ParticleData {
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 size;
	float rotation;
	float lifetime;
	float maxLifetime;
	glm::vec3 velocity;
	float age;
	bool active;

	ParticleData()
		: position(0.0f)
		, color(1.0f)
		, size(1.0f, 1.0f)
		, rotation(0.0f)
		, lifetime(0.0f)
		, maxLifetime(5.0f)
		, velocity(0.0f)
		, age(0.0f)
		, active(false)
	{}
};

class ParticleInstancedRenderer {
public:
	ParticleInstancedRenderer(size_t maxParticles = 1000)
		: maxParticles(maxParticles)
		, activeParticles(0)
	{
		Initialize();
	}

	~ParticleInstancedRenderer() {
		Cleanup();
	}

	void Initialize() {
		particleData.resize(maxParticles);

		const float quadVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		const unsigned int quadIndices[] = {
			0, 1, 2,
			0, 2, 3
		};

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glGenBuffers(1, &instanceVBO);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(InstanceData), nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, position));
		glVertexAttribDivisor(2, 1);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, color));
		glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, size));
		glVertexAttribDivisor(4, 1);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, rotation));
		glVertexAttribDivisor(5, 1);

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, lifetime));
		glVertexAttribDivisor(6, 1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	int AddParticle(const ParticleData& particle) {
		if (activeParticles >= maxParticles) {
			return -1;
		}

		size_t index = 0;
		for (; index < particleData.size(); ++index) {
			if (!particleData[index].active) {
				break;
			}
		}

		if (index >= maxParticles) {
			return -1;
		}

		particleData[index] = particle;
		particleData[index].active = true;
		activeParticles++;

		return static_cast<int>(index);
	}

	void UpdateParticle(size_t index, const ParticleData& particle) {
		if (index < maxParticles && particleData[index].active) {
			particleData[index] = particle;
			particleData[index].active = true;
		}
	}

	void RemoveParticle(size_t index) {
		if (index < maxParticles && particleData[index].active) {
			particleData[index].active = false;
			activeParticles--;
		}
	}

	void UpdateAndRender(float deltaTime) {
		std::vector<InstanceData> instances;
		instances.reserve(activeParticles);

		for (size_t i = 0; i < particleData.size(); ++i) {
			if (!particleData[i].active) {
				continue;
			}

			particleData[i].age += deltaTime;
			particleData[i].position += particleData[i].velocity * deltaTime;

			if (particleData[i].age >= particleData[i].maxLifetime) {
				particleData[i].active = false;
				activeParticles--;
				continue;
			}

			float lifetimeFraction = particleData[i].age / particleData[i].maxLifetime;

			InstanceData instance;
			instance.position = particleData[i].position;
			instance.color = particleData[i].color;
			instance.size = particleData[i].size;
			instance.rotation = particleData[i].rotation;
			instance.lifetime = lifetimeFraction;

			instances.push_back(instance);
		}

		if (!instances.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size() * sizeof(InstanceData), instances.data());

			glBindVertexArray(vao);
			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(instances.size()));
			glBindVertexArray(0);
		}
	}

	void Cleanup() {
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		glDeleteBuffers(1, &instanceVBO);
		glDeleteVertexArrays(1, &vao);
	}

	size_t GetActiveParticleCount() const {
		return activeParticles;
	}

	size_t GetMaxParticles() const {
		return maxParticles;
	}

private:
	struct InstanceData {
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 size;
		float rotation;
		float lifetime;
	};

	GLuint vao, vbo, ebo, instanceVBO;
	std::vector<ParticleData> particleData;
	size_t maxParticles;
	size_t activeParticles;
};