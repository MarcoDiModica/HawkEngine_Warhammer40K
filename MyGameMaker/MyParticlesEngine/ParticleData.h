#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

struct ParticleData {
	bool playOnAwake;
	float duration;
	glm::vec3 position;
	glm::vec4 color;
	glm::vec4 endColor;
	glm::vec2 size;
	glm::vec3 gravity;
	glm::vec2 endSize;
	bool randomRotation;
	float rotation;
	float startRotation;
	float endRotation;
	float lifetime;
	float maxLifetime;
	glm::vec3 velocity;
	glm::vec3 endVelocity;
	float age;
	bool active;
	glm::vec2 spriteOffset;
	glm::vec2 spriteSize;
	glm::vec2 sheetSize;
	bool useAnimation;
	float indexTimer;
	int animIndex;
	float animSpeed;

	ParticleData()
		: playOnAwake(false)
		, duration(1.0f)
		, position(0.0f)
		, color(1.0f)
		, endColor(1.0f)
		, size(1.0f, 1.0f)
		, endSize(1.0f, 1.0f)
		, rotation(0.0f)
		, lifetime(0.0f)
		, maxLifetime(5.0f)
		, velocity(0.0f)
		, endVelocity(0.0f)
		, gravity(0.0f, 0.0f, 0.0f)
		, age(0.0f)
		, active(false)
		, spriteOffset(1.0f,1.0f)
		, spriteSize(1.0f, 1.0f)
		, sheetSize(1.0f, 1.0f)
		, useAnimation(false)
		, indexTimer(0.0f)
		, animIndex(0)
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

		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, endColor));
		glVertexAttribDivisor(7, 1);

		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, endSize));
		glVertexAttribDivisor(8, 1);

		glEnableVertexAttribArray(9);
		glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, spriteOffset));
		glVertexAttribDivisor(9, 1);

		glEnableVertexAttribArray(10);
		glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, spriteSize));
		glVertexAttribDivisor(10, 1);

		glEnableVertexAttribArray(11);
		glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, sheetSize));
		glVertexAttribDivisor(11, 1);

		glEnableVertexAttribArray(12);
		glVertexAttribPointer(12, 1, GL_INT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, useAnimation));
		glVertexAttribDivisor(12, 1);
		
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
		//particleData[index].velocity += new glm::vec3(gravity,gravity,gravity);
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

	int CalculateMaxIndex(const glm::vec2& sheetSize, const glm::vec2& spriteSize) {
		int columns = static_cast<int>(sheetSize.x / spriteSize.x);
		int rows = static_cast<int>(sheetSize.y / spriteSize.y);
		return columns * rows - 1; // Restamos 1 porque los índices empiezan en 0
	}

	glm::vec2 CalculateSpriteOffset(int index, const glm::vec2& sheetSize, const glm::vec2& spriteSize) {
		int columns = static_cast<int>(sheetSize.x / spriteSize.x);
		int rows = static_cast<int>(sheetSize.y / spriteSize.y);

		int currentColumn = index % columns;
		int currentRow = index / columns;

		float offsetX = currentColumn * spriteSize.x;
		float offsetY = currentRow * spriteSize.y;

		return glm::vec2(offsetX, offsetY);
	}

	void UpdateAndRender(float deltaTime) {
		std::vector<InstanceData> instances;
		instances.reserve(activeParticles);

		for (size_t i = 0; i < particleData.size(); ++i) {
			if (!particleData[i].active) {
				continue;
			}

			particleData[i].velocity += particleData[i].gravity * deltaTime;
			particleData[i].age += deltaTime;
			particleData[i].position += particleData[i].velocity * deltaTime;

			

			if (particleData[i].age >= particleData[i].maxLifetime) {
				particleData[i].active = false;
				activeParticles--;
				continue;
			}

			float lifetimeFraction = particleData[i].age / particleData[i].maxLifetime;

			if (glm::length(particleData[i].gravity) == 0.0f)
			{
				particleData[i].velocity = glm::mix(particleData[i].velocity, particleData[i].endVelocity, lifetimeFraction);
			}
			
			if (!particleData[i].randomRotation)
			{
				particleData[i].rotation = glm::lerp(particleData[i].rotation, particleData[i].endRotation*360.0f, lifetimeFraction);
			}
		

			if (particleData[i].useAnimation) 
			{
				particleData[i].indexTimer += deltaTime;

				if (particleData[i].indexTimer >= particleData[i].animSpeed)
				{
					if (particleData[i].indexTimer >= CalculateMaxIndex(particleData[i].sheetSize, particleData[i].spriteSize))
					{
						particleData[i].indexTimer = 0;
						particleData[i].animIndex = 0;
					}
					else
					{
						particleData[i].animIndex++;
					}
					particleData[i].indexTimer = 0.0f;
					particleData[i].spriteOffset = CalculateSpriteOffset(particleData[i].animIndex, particleData[i].sheetSize, particleData[i].spriteSize);

				}
			}
			else 
			{
				particleData[i].spriteOffset = glm::vec2(0.0f, 0.0f);
				particleData[i].spriteSize = particleData[i].sheetSize;
			}
			
		
			

			InstanceData instance;
			instance.playOnAwake = particleData[i].playOnAwake;
			instance.duration = particleData[i].duration;
			instance.position = particleData[i].position;
			instance.color = particleData[i].color;
			instance.endColor = particleData[i].endColor;
			instance.size = particleData[i].size;
			instance.endSize = particleData[i].endSize;
			instance.gravity = particleData[i].gravity;
			instance.rotation = particleData[i].rotation;
			instance.endSpeed = particleData[i].endVelocity;
			instance.lifetime = lifetimeFraction;
			instance.spriteOffset = particleData[i].spriteOffset;
			instance.spriteSize = particleData[i].spriteSize;
			instance.sheetSize = particleData[i].sheetSize;
			instance.useAnimation = particleData[i].useAnimation;
			instance.indexTimer = particleData[i].indexTimer;
			instance.animIndex = particleData[i].animIndex;

			instances.push_back(instance);
		}

		if (!instances.empty()) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);

			glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size() * sizeof(InstanceData), instances.data());

			glBindVertexArray(vao);
			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(instances.size()));
			glBindVertexArray(0);

			glDepthMask(GL_TRUE);
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
		bool playOnAwake;
		float duration;
		glm::vec3 position;
		glm::vec4 color;
		glm::vec4 endColor;
		glm::vec3 gravity;
		glm::vec2 size;
		glm::vec2 endSize;
		glm::vec3 endSpeed;
		float rotation;
		float lifetime;
		glm::vec2 spriteOffset;
		glm::vec2 spriteSize;
		glm::vec2 sheetSize;
		bool useAnimation;
		float indexTimer;
		int animIndex;
	};

	GLuint vao, vbo, ebo, instanceVBO;
	std::vector<ParticleData> particleData;
	size_t maxParticles;
	size_t activeParticles;

};