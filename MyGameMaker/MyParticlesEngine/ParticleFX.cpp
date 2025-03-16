#include "ParticleFX.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/TransformComponent.h"
#include "ParticleShader.h"
#include "../MyGameEditor/Root.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/CameraComponent.h"

namespace ParticlePresets {
	const ParticlePreset Smoke = {
		ParticleType::SMOKE,
		glm::vec3(0.8f, 0.8f, 0.8f),   // Start color (light gray)
		glm::vec3(0.2f, 0.2f, 0.2f),   // End color (dark gray)
		0.7f,                          // Alpha start
		0.0f,                          // Alpha end
		0.5f,                          // Size start
		2.0f,                          // Size end
		2.0f,                          // Min lifetime
		4.0f,                          // Max lifetime
		0.5f,                          // Min speed
		1.5f,                          // Max speed
		-0.1f,                         // Gravity (negative for upward)
		0.2f,                          // Rotation speed
		15.0f,                         // Emission rate (particles per second)
		EmitterShape::CONE,            // Shape
		0.2f,                          // Cone base radius
		1.0f,                          // Cone height
		20.0f                          // Cone angle in degrees
	};

	const ParticlePreset Fire = {
		ParticleType::FIRE,
		glm::vec3(1.0f, 0.7f, 0.0f),   // Start color (orange)
		glm::vec3(1.0f, 0.0f, 0.0f),   // End color (red)
		0.9f,                          // Alpha start
		0.0f,                          // Alpha end
		0.8f,                          // Size start
		0.1f,                          // Size end
		0.5f,                          // Min lifetime
		1.5f,                          // Max lifetime
		1.0f,                          // Min speed
		2.0f,                          // Max speed
		-0.5f,                         // Gravity (negative for upward)
		0.5f,                          // Rotation speed
		40.0f,                         // Emission rate (particles per second)
		EmitterShape::CONE,            // Shape
		0.3f,                          // Cone base radius
		1.0f,                          // Cone height
		30.0f                          // Cone angle in degrees
	};

	const ParticlePreset MuzzleFlash = {
		ParticleType::MUZZLE_FLASH,
		glm::vec3(1.0f, 0.9f, 0.5f),   // Start color (bright yellow)
		glm::vec3(1.0f, 0.5f, 0.0f),   // End color (orange)
		1.0f,                          // Alpha start
		0.0f,                          // Alpha end
		0.3f,                          // Size start
		0.1f,                          // Size end
		0.05f,                         // Min lifetime
		0.15f,                         // Max lifetime
		5.0f,                          // Min speed
		10.0f,                         // Max speed
		0.0f,                          // Gravity
		0.0f,                          // Rotation speed
		100.0f,                        // Emission rate (particles per second)
		EmitterShape::CONE,            // Shape
		0.05f,                         // Cone base radius
		0.2f,                          // Cone height
		20.0f                          // Cone angle in degrees
	};

	const ParticlePreset Dust = {
		ParticleType::DEFAULT,
		glm::vec3(0.76f, 0.7f, 0.5f),  // Start color (tan)
		glm::vec3(0.76f, 0.7f, 0.5f),  // End color (tan)
		0.6f,                          // Alpha start
		0.0f,                          // Alpha end
		0.2f,                          // Size start
		0.8f,                          // Size end
		1.0f,                          // Min lifetime
		3.0f,                          // Max lifetime
		0.2f,                          // Min speed
		0.8f,                          // Max speed
		0.1f,                          // Gravity (slight downward)
		0.3f,                          // Rotation speed
		10.0f,                         // Emission rate (particles per second)
		EmitterShape::CIRCLE,          // Shape
		0.5f,                          // Circle radius
		0.0f,                          // Unused
		0.0f                           // Unused
	};
}

ParticleFX::ParticleFX(GameObject* owner)
	: Component(owner)
	, emitterShape(EmitterShape::POINT)
	, emissionRate(10.0f)
	, particlesPerSecond(10.0f)
	, timeSinceLastEmit(0.0f)
	, minLifetime(1.0f)
	, maxLifetime(3.0f)
	, minSpeed(1.0f)
	, maxSpeed(3.0f)
	, startSize(1.0f)
	, endSize(1.0f)
	, startColor(1.0f, 1.0f, 1.0f)
	, endColor(1.0f, 1.0f, 1.0f)
	, startAlpha(1.0f)
	, endAlpha(0.0f)
	, rotationSpeed(0.0f)
	, gravity(0.0f)
	, shapeParam1(0.5f)
	, shapeParam2(1.0f)
	, shapeParam3(0.0f)
	, isPlaying(false)
	, isPaused(false)
	, isOneShot(false)
	, burstEmitted(false)
	, rng(std::random_device()())
	, dist01(0.0f, 1.0f)
{
	name = "ParticleFX";

	material = std::make_shared<ParticleMaterial>();
	renderer = std::make_unique<ParticleInstancedRenderer>(1000);

	if (owner->GetTransform()) {
		position = owner->GetTransform()->GetPosition();
		rotation = owner->GetTransform()->GetRotation();
		scale = owner->GetTransform()->GetScale();
	}
}

void ParticleFX::Start() {
	if (owner->GetTransform()) {
		position = owner->GetTransform()->GetPosition();
		rotation = owner->GetTransform()->GetRotation();
		scale = owner->GetTransform()->GetScale();
	}

	if (!isOneShot) {
		isPlaying = true;
	}
}

void ParticleFX::Update(float deltaTime) {
	GLint lastProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);

	GLint lastVAO;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVAO);

	GLint lastTexture;
	glActiveTexture(GL_TEXTURE0);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);

	GLboolean lastDepthTest = glIsEnabled(GL_DEPTH_TEST);
	GLboolean lastBlend = glIsEnabled(GL_BLEND);
	GLint lastBlendSrc, lastBlendDst;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &lastBlendSrc);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &lastBlendDst);

	if (owner->GetTransform()) {
		position = owner->GetTransform()->GetPosition();
	}

	timeSinceLastEmit += deltaTime;
	float emitInterval = 1.0f / particlesPerSecond;

	while (timeSinceLastEmit >= emitInterval && isPlaying && !isPaused) {
		EmitParticle();
		timeSinceLastEmit -= emitInterval;
	}

	glm::vec3 cameraPosition = Application->camera->GetTransform().GetPosition();
	glm::vec3 cameraUp = Application->camera->GetTransform().GetUp();

	glm::mat4 modelMatrix = owner->GetTransform()->GetMatrix();
	glm::mat4 viewMatrix = Application->camera->view();
	glm::mat4 projMatrix = Application->camera->projection();

	material->ApplyShader(modelMatrix, viewMatrix, projMatrix);

	if (material) {
		ParticleShader* particleShader = static_cast<ParticleShader*>(
			ShaderManager::GetInstance().GetShader(material->GetShaderType()));

		if (particleShader) {
			particleShader->SetCameraPosition(cameraPosition);
			particleShader->SetCameraUp(cameraUp);
		}
	}

	renderer->UpdateAndRender(deltaTime);

	glUseProgram(lastProgram);
	glBindVertexArray(lastVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lastTexture);

	if (lastDepthTest) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);

	if (lastBlend) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);

	glBlendFunc(lastBlendSrc, lastBlendDst);
}

void ParticleFX::RenderGameView() {
	GLint lastProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);

	GLint lastVAO;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVAO);

	GLint lastTexture;
	glActiveTexture(GL_TEXTURE0);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);

	GLboolean lastDepthTest = glIsEnabled(GL_DEPTH_TEST);
	GLboolean lastBlend = glIsEnabled(GL_BLEND);
	GLint lastBlendSrc, lastBlendDst;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &lastBlendSrc);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &lastBlendDst);

	glm::vec3 cameraPosition = Application->root->mainCamera->GetTransform()->GetPosition();
	glm::vec3 cameraUp = Application->root->mainCamera->GetTransform()->GetUp();

	glm::mat4 modelMatrix = owner->GetTransform()->GetMatrix();
	glm::mat4 viewMatrix = Application->root->mainCamera->GetComponent<CameraComponent>()->view();
	glm::mat4 projMatrix = Application->root->mainCamera->GetComponent<CameraComponent>()->projection();

	material->ApplyShader(modelMatrix, viewMatrix, projMatrix);

	if (material) {
		ParticleShader* particleShader = static_cast<ParticleShader*>(
			ShaderManager::GetInstance().GetShader(material->GetShaderType()));

		if (particleShader) {
			particleShader->SetCameraPosition(cameraPosition);
			particleShader->SetCameraUp(cameraUp);
		}
	}

	renderer->UpdateAndRender(0.0f);

	glUseProgram(lastProgram);
	glBindVertexArray(lastVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lastTexture);

	if (lastDepthTest) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);

	if (lastBlend) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);

	glBlendFunc(lastBlendSrc, lastBlendDst);
}

void ParticleFX::Destroy() {
	renderer->Cleanup();
}

std::unique_ptr<Component> ParticleFX::Clone(GameObject* owner) {
	auto clone = std::make_unique<ParticleFX>(owner);

	clone->material = material;
	clone->emitterShape = emitterShape;
	clone->emissionRate = emissionRate;
	clone->particlesPerSecond = particlesPerSecond;
	clone->minLifetime = minLifetime;
	clone->maxLifetime = maxLifetime;
	clone->minSpeed = minSpeed;
	clone->maxSpeed = maxSpeed;
	clone->startSize = startSize;
	clone->endSize = endSize;
	clone->startColor = startColor;
	clone->endColor = endColor;
	clone->startAlpha = startAlpha;
	clone->endAlpha = endAlpha;
	clone->rotationSpeed = rotationSpeed;
	clone->gravity = gravity;
	clone->shapeParam1 = shapeParam1;
	clone->shapeParam2 = shapeParam2;
	clone->shapeParam3 = shapeParam3;
	clone->isOneShot = isOneShot;

	return clone;
}

void ParticleFX::EmitParticle() {
	ParticleData particle;

	particle.maxLifetime = minLifetime + dist01(rng) * (maxLifetime - minLifetime);
	particle.lifetime = 0.0f;

	particle.position = position + GenerateRandomPosition();

	particle.velocity = GenerateRandomVelocity();

	particle.color = glm::vec4(startColor, startAlpha);

	particle.size = glm::vec2(startSize, startSize);

	particle.rotation = dist01(rng) * 360.0f;

	renderer->AddParticle(particle);
}

glm::vec3 ParticleFX::GenerateRandomPosition() {
	switch (emitterShape) {
	case EmitterShape::POINT:
		return glm::vec3(0.0f);

	case EmitterShape::SPHERE: {
		// Random direction within a sphere
		float theta = dist01(rng) * 2.0f * glm::pi<float>();
		float phi = acos(2.0f * dist01(rng) - 1.0f);
		float r = shapeParam1 * std::cbrt(dist01(rng));

		return {
			r * sin(phi) * cos(theta),
			r * sin(phi) * sin(theta),
			r * cos(phi)
		};
	}

	case EmitterShape::CONE: {
		float radius = shapeParam1 * sqrt(dist01(rng));
		float angle = dist01(rng) * 2.0f * glm::pi<float>();
		float height = shapeParam2 * dist01(rng);

		return {
			radius * cos(angle),
			height,
			radius * sin(angle)
		};
	}

	case EmitterShape::BOX: {
		return {
			(dist01(rng) - 0.5f) * shapeParam1,  // Width
			(dist01(rng) - 0.5f) * shapeParam2,  // Height
			(dist01(rng) - 0.5f) * shapeParam3   // Depth
		};
	}

	case EmitterShape::CIRCLE: {
		float radius = shapeParam1 * sqrt(dist01(rng));
		float angle = dist01(rng) * 2.0f * glm::pi<float>();

		return {
			radius * cos(angle),
			0.0f,
			radius * sin(angle)
		};
	}
	}

	return glm::vec3(0.0f);
}

glm::vec3 ParticleFX::GenerateRandomVelocity() {
	float speed = minSpeed + dist01(rng) * (maxSpeed - minSpeed);

	switch (emitterShape) {
	case EmitterShape::POINT: {
		float theta = dist01(rng) * 2.0f * glm::pi<float>();
		float phi = acos(2.0f * dist01(rng) - 1.0f);

		return speed * glm::vec3(
			sin(phi) * cos(theta),
			sin(phi) * sin(theta),
			cos(phi)
		);
	}

	case EmitterShape::SPHERE: {
		glm::vec3 direction = glm::normalize(GenerateRandomPosition());
		return speed * direction;
	}

	case EmitterShape::CONE: {
		float coneAngleRad = glm::radians(shapeParam3);
		float theta = dist01(rng) * 2.0f * glm::pi<float>();
		float phi = dist01(rng) * coneAngleRad;

		return speed * glm::vec3(
			sin(phi) * cos(theta),
			cos(phi),
			sin(phi) * sin(theta)
		);
	}

	case EmitterShape::BOX: {
		float theta = dist01(rng) * 2.0f * glm::pi<float>();
		float phi = acos(2.0f * dist01(rng) - 1.0f);

		return speed * glm::vec3(
			sin(phi) * cos(theta),
			sin(phi) * sin(theta),
			cos(phi)
		);
	}

	case EmitterShape::CIRCLE: {
		return speed * glm::vec3(0.0f, 1.0f, 0.0f);
	}
	}

	return {0.0f, speed, 0.0f};
}

void ParticleFX::ApplyPreset(const ParticlePreset& preset) {
	material->SetParticleType(preset.type);
	startColor = preset.colorStart;
	endColor = preset.colorEnd;
	startAlpha = preset.alphaStart;
	endAlpha = preset.alphaEnd;
	startSize = preset.sizeStart;
	endSize = preset.sizeEnd;
	minLifetime = preset.minLifetime;
	maxLifetime = preset.maxLifetime;
	minSpeed = preset.minSpeed;
	maxSpeed = preset.maxSpeed;
	gravity = preset.gravity;
	rotationSpeed = preset.rotationSpeed;
	emissionRate = preset.emissionRate;
	particlesPerSecond = preset.emissionRate;
	emitterShape = preset.shape;
	shapeParam1 = preset.shapeParam1;
	shapeParam2 = preset.shapeParam2;
	shapeParam3 = preset.shapeParam3;
}

void ParticleFX::ConfigureSmoke() {
	ApplyPreset(ParticlePresets::Smoke);
}

void ParticleFX::ConfigureFire() {
	ApplyPreset(ParticlePresets::Fire);
}

void ParticleFX::ConfigureMuzzleFlash() {
	ApplyPreset(ParticlePresets::MuzzleFlash);
	SetOneShot(true);
}

void ParticleFX::ConfigureDust() {
	ApplyPreset(ParticlePresets::Dust);
}

void ParticleFX::EmitBurst(int count) {
	for (int i = 0; i < count; ++i) {
		EmitParticle();
	}
}

void ParticleFX::Play() {
	isPlaying = true;
	isPaused = false;

	if (isOneShot) {
		burstEmitted = false;
	}
}

void ParticleFX::Stop() {
	isPlaying = false;
	isPaused = false;

	renderer->Cleanup();
	renderer = std::make_unique<ParticleInstancedRenderer>(1000);
}

void ParticleFX::Pause() {
	isPaused = true;
}

bool ParticleFX::IsPlaying() const {
	return isPlaying && !isPaused;
}

void ParticleFX::SetEmitterShape(EmitterShape shape) {
	emitterShape = shape;
}

void ParticleFX::SetEmissionRate(float particlesPerSecond) {
	this->emissionRate = particlesPerSecond;
	this->particlesPerSecond = particlesPerSecond;
}

void ParticleFX::SetParticleLifetime(float min, float max) {
	minLifetime = min;
	maxLifetime = max;
}

void ParticleFX::SetParticleSpeed(float min, float max) {
	minSpeed = min;
	maxSpeed = max;
}

void ParticleFX::SetParticleSize(float startSize, float endSize) {
	this->startSize = startSize;
	this->endSize = endSize;
}

void ParticleFX::SetParticleColor(const glm::vec3& startColor, const glm::vec3& endColor) {
	this->startColor = startColor;
	this->endColor = endColor;
}

void ParticleFX::SetParticleAlpha(float startAlpha, float endAlpha) {
	this->startAlpha = startAlpha;
	this->endAlpha = endAlpha;
}

void ParticleFX::SetParticleRotation(float rotationSpeed) {
	this->rotationSpeed = rotationSpeed;
}

void ParticleFX::SetGravity(float gravity) {
	this->gravity = gravity;
}

void ParticleFX::SetTexture(const std::string& texturePath) {
	auto image = std::make_shared<Image>();
	image->LoadTexture(texturePath);
	material->setImage(image);
}

void ParticleFX::SetColorGradient(const std::string& texturePath) {
	auto image = std::make_shared<Image>();
	image->LoadTexture(texturePath);
	material->SetColorGradientMap(image);
}

void ParticleFX::SetBillboardType(int billboardType) {
	material->SetBillboardType(billboardType);
}

void ParticleFX::SetOneShot(bool oneShot) {
	isOneShot = oneShot;
	burstEmitted = false;
}

void ParticleFX::SetShapeParameters(float param1, float param2, float param3) {
	shapeParam1 = param1;
	shapeParam2 = param2;
	shapeParam3 = param3;
}