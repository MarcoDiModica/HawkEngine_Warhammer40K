#include "ParticleFX.h"
#include "../MyGameEditor/App.h"
#include "../MyGameEngine/TransformComponent.h"
#include "ParticleShader.h"
#include "../MyGameEditor/Root.h"
#include "../MyGameEditor/App.h"
#include <MyScriptingEngine/MonoManager.h>
#include <mono/metadata/debug-helpers.h>
#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEngine/CameraComponent.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace ParticlePresets {
	const ParticlePreset Smoke = {
		ParticleType::SMOKE,
		true,						   // PlayOnAwake
		5.0f,						   // Duration (only if one-shot)
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
		4.0f,						   // End Speed
		glm::vec3(0.0f,-0.1f,0.0f),	   // Gravity (negative for upward)
		0.2f,                          // Rotation speed
		15.0f,                         // Emission rate (particles per second)
		EmitterShape::CONE,            // Shape
		0.2f,                          // Cone base radius
		1.0f,                          // Cone height
		20.0f,                         // Cone angle in degrees
		glm::vec2(256,256),			   // Sprite size
		true,						   // Use animation
		true,						   // Random animation Index
		0.5f,						   // Animation speed
		0.0f,						   // Start rotation
		true,						   // Random rotation
		0.5f,						   // Min scale
		0.0f,						   // Max scale
		"Assets/Textures/Smoke30Frames.png", // Texture path
		false						   // Is Local Space
	};

	const ParticlePreset Fire = {
		ParticleType::FIRE,
		true,						   // PlayOnAwake
		5.0f,						   // Duration (only if one-shot)
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
		4.0f,						   // End Speed
		glm::vec3(0.0f,-0.5f,0.0f),	   // Gravity (negative for upward)
		0.5f,                          // Rotation speed
		40.0f,                         // Emission rate (particles per second)
		EmitterShape::CONE,            // Shape
		0.3f,                          // Cone base radius
		1.0f,                          // Cone height
		30.0f,                         // Cone angle in degrees
		glm::vec2(256,256),         // Sprite size
		true,						   // Use animation
		true,						   // Random animation Index
		0.5f,						   // Animation speed
		0.0f,						   // Start rotation
		true,						   // Random rotation
		0.5f,						   // Min scale
		0.0f,						   // Max scale
		"Assets/Textures/Smoke30Frames.png", // Texture path
		false				   // Is Local Space
	};

	const ParticlePreset MuzzleFlash = {
		ParticleType::MUZZLE_FLASH,
		false,						   // PlayOnAwake
		0.1f,						   // Duration (only if one-shot)
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
		4.0f,						   // End Speed
		glm::vec3(0.0f,0.0f,0.0f),	   // Gravity
		0.0f,                          // Rotation speed
		100.0f,                        // Emission rate (particles per second)
		EmitterShape::CONE,            // Shape
		0.05f,                         // Cone base radius
		0.2f,                          // Cone height
		20.0f,                         // Cone angle in degrees
		glm::vec2(256,256),         // Sprite size
		true,						   // Use animation
		true,						   // Random animation Index
		0.5f,						   // Animation speed
		0.0f,						   // Start rotation
		true,						   // Random rotation
		0.5f,						   // Min scale
		0.0f,						   // Max scale
		"Assets/Textures/Smoke30Frames.png", // Texture path
		false						   // Is Local Space
	};

	const ParticlePreset Dust = {
		ParticleType::DEFAULT,
		true,						   // PlayOnAwake
		5.0f,						   // Duration (only if one-shot)
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
		4.0f,						   // End Speed
		glm::vec3(0.0f,0.1f,0.0f),     // Gravity (slight downward)
		0.3f,                          // Rotation speed
		10.0f,                         // Emission rate (particles per second)
		EmitterShape::CIRCLE,          // Shape
		0.5f,                          // Circle radius
		0.0f,                          // Unused
		0.0f,                          // Unused
		glm::vec2(256,256),         // Sprite size
		true,						   // Use animation
		true,						   // Random animation Index
		0.5f,						   // Animation speed
		0.0f,						   // Start rotation
		true,						   // Random rotation
		0.5f,						   // Min scale
		0.0f,						   // Max scale
		"Assets/Textures/Smoke30Frames.png", // Texture path
		false						   // Is Local Space

	};

	const ParticlePreset Explosion = {
		ParticleType::EXPLOSION,
		false,						   // PlayOnAwake
		0.2f,						   // Duration (only if one-shot)
		glm::vec3(1.0f, 0.5f, 0.0f),   // Start color (orange)
		glm::vec3(0.5f, 1.0f, 0.0f),   // End color (dark red)
		1.0f,                          // Alpha start
		0.0f,                          // Alpha end
		1.0f,                          // Size start
		3.0f,                          // Size end
		0.2f,                          // Min lifetime
		1.0f,                          // Max lifetime
		5.0f,                          // Min speed
		10.0f,                         // Max speed
		4.0f,						   // End Speed
		glm::vec3(0.0f,-0.5f,0.0f),	   // Gravity (negative for upward)
		1.0f,                          // Rotation speed
		100.0f,                        // Emission rate (particles per second)
		EmitterShape::SPHERE,          // Shape
		1.0f,                          // Sphere radius
		0.0f,                          // Unused
		0.0f,                          // Unused
		glm::vec2(256,256),         // Sprite size
		true,						   // Use animation
		false,						   // Random animation Index
		0.5f,						   // Animation speed
		0.0f,						   // Start rotation
		true,						   // Random rotation
		0.5f,						   // Min scale
		0.0f,						   // Max scale
		"Assets/Textures/Smoke30Frames.png", // Texture path
		false						   // Is Local Space
	};

	const ParticlePreset Flame = {
		ParticleType::FLAME,
		true,						   // PlayOnAwake
		1.0f,						   // Duration (only if one-shot)
		glm::vec3(1,1,1),   // Start color (light gray)
		glm::vec3(1,1,1),   // End color (dark gray)
		1.0f,                          // Alpha start
		1.0f,                          // Alpha end
		1.0f,                          // Size start
		1.0f,                          // Size end
		1.0f,                          // Min lifetime
		1.0f,                          // Max lifetime
		0.0f,                          // Min speed
		0.0f,                          // Max speed
		0.0f,						   // End Speed
		glm::vec3(0.0f,-0.001f,0.0f),	   // Gravity (negative for upward)
		0,                          // Rotation speed
		1.0f,                         // Emission rate (particles per second)
		EmitterShape::POINT,            // Shape
		0.2f,                          // Cone base radius
		1.0f,                          // Cone height
		20.0f,                         // Cone angle in degrees
		glm::vec2(385,217),			   // Sprite size
		true,						   // Use animation
		true,						   // Random animation Index
		0.05f,						   // Animation speed
		0.0f,						   // Start rotation
		false,						   // Random rotation
		1.0f,						   // Min scale
		1.0f,						   // Max scale
		"Assets/Textures/fire_spritesheet.png", // Texture path
		false						   // Is Local Space
	};

	const ParticlePreset Environment_Smoke = {
		ParticleType::ENVIRONMENT_SMOKE,
		true,						   // PlayOnAwake
		5,						   // Duration (only if one-shot)
		glm::vec3(1,1,1),   // Start color (light gray)
		glm::vec3(1,1,1),   // End color (dark gray)
		1.0f,                          // Alpha start
		0,                          // Alpha end
		0.5f,                          // Size start
		3.0f,                          // Size end
		3.0f,                          // Min lifetime
		4.0f,                          // Max lifetime
		0.0f,                          // Min speed
		0.0f,                          // Max speed
		0.0f,						   // End Speed
		glm::vec3(0.0f,1,0.0f),	   // Gravity (negative for upward)
		0,                          // Rotation speed
		15,                         // Emission rate (particles per second)
		EmitterShape::POINT,            // Shape
		0.2f,                          // Cone base radius
		1.0f,                          // Cone height
		20.0f,                         // Cone angle in degrees
		glm::vec2(193,109),			   // Sprite size
		true,						   // Use animation
		true,						   // Random animation Index
		0.005f,						   // Animation speed
		0.0f,						   // Start rotation
		true,						   // Random rotation
		1.0f,						   // Min scale
		1.0f,						   // Max scale
		"Assets/Textures/smoke_spritesheet.png", // Texture path
		false						   // Is Local Space
	};

	const ParticlePreset Environment_Explosion = {
		ParticleType::ENVIRONMENT_EXPLOSION,
		true,						   // PlayOnAwake
		5,						   // Duration (only if one-shot)
		glm::vec3(1,1,1),   // Start color (light gray)
		glm::vec3(1,1,1),   // End color (dark gray)
		1.0f,                          // Alpha start
		1.0f,                          // Alpha end
		1,                          // Size start
		1,                          // Size end
		1,                          // Min lifetime
		1,                          // Max lifetime
		0.0f,                          // Min speed
		0.0f,                          // Max speed
		0.0f,						   // End Speed
		glm::vec3(0.0f,0.001f,0.0f),	   // Gravity (negative for upward)
		0,                          // Rotation speed
		1,                         // Emission rate (particles per second)
		EmitterShape::POINT,            // Shape
		0.2f,                          // Cone base radius
		1.0f,                          // Cone height
		20.0f,                         // Cone angle in degrees
		glm::vec2(640,360),			   // Sprite size
		true,						   // Use animation
		false,						   // Random animation Index
		0.05f,						   // Animation speed
		0.0f,						   // Start rotation
		true,						   // Random rotation
		1.0f,						   // Min scale
		1.0f,						   // Max scale
		"Assets/Textures/ixplosion.png", // Texture path
		false						   // Is Local Space
	};


	const ParticlePreset Riffle_Shot = {
		ParticleType::RIFFLE_SHOT,
		true,						   // PlayOnAwake
		5,						   // Duration (only if one-shot)
		glm::vec3(1,1,1),   // Start color (light gray)
		glm::vec3(1,1,1),   // End color (dark gray)
		1.0f,                          // Alpha start
		1.0f,                          // Alpha end
		1,                          // Size start
		1,                          // Size end
		0.25f,                          // Min lifetime
		0.25f,                          // Max lifetime
		0.0f,                          // Min speed
		0.0f,                          // Max speed
		0.0f,						   // End Speed
		glm::vec3(0.0f,0.001f,0.0f),	   // Gravity (negative for upward)
		0,                          // Rotation speed
		1,                         // Emission rate (particles per second)
		EmitterShape::POINT,            // Shape
		0.2f,                          // Cone base radius
		1.0f,                          // Cone height
		20.0f,                         // Cone angle in degrees
		glm::vec2(1920,1080),			   // Sprite size
		true,						   // Use animation
		false,						   // Random animation Index
		0.05f,						   // Animation speed
		0.0f,						   // Start rotation
		false,						   // Random rotation
		1.0f,						   // Min scale
		1.0f,						   // Max scale
		"Assets/Textures/muzzle.png", // Texture path
		true						   // Is Local Space
	};

	const ParticlePreset Shotgun_Shot = {
		ParticleType::SHOTGUN_SHOT,
		true,						   // PlayOnAwake
		5,						   // Duration (only if one-shot)
		glm::vec3(1,1,1),   // Start color (light gray)
		glm::vec3(1,1,1),   // End color (dark gray)
		1.0f,                          // Alpha start
		1.0f,                          // Alpha end
		1,                          // Size start
		1,                          // Size end
		0.25f,                          // Min lifetime
		0.25f,                          // Max lifetime
		0.0f,                          // Min speed
		0.0f,                          // Max speed
		0.0f,						   // End Speed
		glm::vec3(0.0f,0.001f,0.0f),	   // Gravity (negative for upward)
		0,                          // Rotation speed
		1,                         // Emission rate (particles per second)
		EmitterShape::POINT,            // Shape
		0.2f,                          // Cone base radius
		1.0f,                          // Cone height
		20.0f,                         // Cone angle in degrees
		glm::vec2(1024,778),			   // Sprite size
		true,						   // Use animation
		false,						   // Random animation Index
		0.05f,						   // Animation speed
		0.0f,						   // Start rotation
		false,						   // Random rotation
		1.0f,						   // Min scale
		1.0f,						   // Max scale
		"Assets/Textures/ShotGun Muzzle Flash_Spritesheet_Yiwei.png", // Texture path
		true						   // Is Local Space
	};

	const ParticlePreset Enemy_Dash = {
		ParticleType::ENEMY_DASH,
		true,						   // PlayOnAwake
		1.0f,						   // Duration (only if one-shot)
		glm::vec3(1,1,1),   // Start color (light gray)
		glm::vec3(1,1,1),   // End color (dark gray)
		1.0f,                          // Alpha start
		1.0f,                          // Alpha end
		2,                          // Size start
		2,                          // Size end
		1,                          // Min lifetime
		1,                          // Max lifetime
		0.0f,                          // Min speed
		0.0f,                          // Max speed
		0.0f,						   // End Speed
		glm::vec3(0.0f,0.001f,0.0f),	   // Gravity (negative for upward)
		0,                          // Rotation speed
		1,                         // Emission rate (particles per second)
		EmitterShape::POINT,            // Shape
		0.2f,                          // Cone base radius
		1.0f,                          // Cone height
		20.0f,                         // Cone angle in degrees
		glm::vec2(640,360),			   // Sprite size
		true,						   // Use animation
		false,						   // Random animation Index
		0.05f,						   // Animation speed
		0.0f,						   // Start rotation
		false,						   // Random rotation
		1.0f,						   // Min scale
		1.0f,						   // Max scale
		"Assets/Textures/EnemyDash.png", // Texture path
		false						   // Is Local Space
	};


	const ParticlePreset Acid_Atk = {
	ParticleType::ACID_ATK,
	true,						   // PlayOnAwake
	5,						   // Duration (only if one-shot)
	glm::vec3(1,1,1),   // Start color (light gray)
	glm::vec3(1,1,1),   // End color (dark gray)
	1.0f,                          // Alpha start
	1.0f,                          // Alpha end
	1,                          // Size start
	1,                          // Size end
	1,                          // Min lifetime
	1,                          // Max lifetime
	0.0f,                          // Min speed
	0.0f,                          // Max speed
	0.0f,						   // End Speed
	glm::vec3(0.0f,0.001f,0.0f),	   // Gravity (negative for upward)
	0,                          // Rotation speed
	1,                         // Emission rate (particles per second)
	EmitterShape::POINT,            // Shape
	0.2f,                          // Cone base radius
	1.0f,                          // Cone height
	20.0f,                         // Cone angle in degrees
	glm::vec2(240,135),			   // Sprite size
	true,						   // Use animation
	true,						   // Random animation Index
	0.05f,						   // Animation speed
	0.0f,						   // Start rotation
	false,						   // Random rotation
	1.0f,						   // Min scale
	1.0f,						   // Max scale
	"Assets/Textures/Acid_Splash.png", // Texture path
	false						   // Is Local Space
	};

	const ParticlePreset Acid_Puddle = {
	ParticleType::ACID_PUDDLE,
	true,						   // PlayOnAwake
	5,						   // Duration (only if one-shot)
	glm::vec3(1,1,1),   // Start color (light gray)
	glm::vec3(1,1,1),   // End color (dark gray)
	1.0f,                          // Alpha start
	1.0f,                          // Alpha end
	1,                          // Size start
	1,                          // Size end
	1,                          // Min lifetime
	1,                          // Max lifetime
	0.0f,                          // Min speed
	0.0f,                          // Max speed
	0.0f,						   // End Speed
	glm::vec3(0.0f,0.001f,0.0f),	   // Gravity (negative for upward)
	0,                          // Rotation speed
	1,                         // Emission rate (particles per second)
	EmitterShape::POINT,            // Shape
	0.2f,                          // Cone base radius
	1.0f,                          // Cone height
	20.0f,                         // Cone angle in degrees
	glm::vec2(480,270),			   // Sprite size
	true,						   // Use animation
	true,						   // Random animation Index
	0.05f,						   // Animation speed
	0.0f,						   // Start rotation
	false,						   // Random rotation
	1.0f,						   // Min scale
	1.0f,						   // Max scale
	"Assets/Textures/acid_puddle.png", // Texture path
	false						   // Is Local Space
	};

	const ParticlePreset Energy_Ball = {
	ParticleType::ENERGY_BALL,
	true,						   // PlayOnAwake
	5,						   // Duration (only if one-shot)
	glm::vec3(1,1,1),   // Start color (light gray)
	glm::vec3(1,1,1),   // End color (dark gray)
	1.0f,                          // Alpha start
	1.0f,                          // Alpha end
	5,                          // Size start
	5,                          // Size end
	1,                          // Min lifetime
	1,                          // Max lifetime
	0.0f,                          // Min speed
	0.0f,                          // Max speed
	0.0f,						   // End Speed
	glm::vec3(0.0f,0.001f,0.0f),	   // Gravity (negative for upward)
	0,                          // Rotation speed
	1,                         // Emission rate (particles per second)
	EmitterShape::POINT,            // Shape
	0.2f,                          // Cone base radius
	1.0f,                          // Cone height
	20.0f,                         // Cone angle in degrees
	glm::vec2(641,361),			   // Sprite size
	true,						   // Use animation
	true,						   // Random animation Index
	0.05f,						   // Animation speed
	0.0f,						   // Start rotation
	false,						   // Random rotation
	1.0f,						   // Min scale
	1.0f,						   // Max scale
	"Assets/Textures/ElectricityBall.png", // Texture path
	true						   // Is Local Space
	};

	const ParticlePreset RailGun_Auto = {
	ParticleType::RAILGUN_AUTO,
	true,						   // PlayOnAwake
	5,						   // Duration (only if one-shot)
	glm::vec3(1,1,1),   // Start color (light gray)
	glm::vec3(1,1,1),   // End color (dark gray)
	1.0f,                          // Alpha start
	1.0f,                          // Alpha end
	1,                          // Size start
	1,                          // Size end
	1,                          // Min lifetime
	1,                          // Max lifetime
	0.0f,                          // Min speed
	0.0f,                          // Max speed
	0.0f,						   // End Speed
	glm::vec3(0.0f,0.001f,0.0f),	   // Gravity (negative for upward)
	0,                          // Rotation speed
	1,                         // Emission rate (particles per second)
	EmitterShape::POINT,            // Shape
	0.2f,                          // Cone base radius
	1.0f,                          // Cone height
	20.0f,                         // Cone angle in degrees
	glm::vec2(434,181),			   // Sprite size
	true,						   // Use animation
	true,						   // Random animation Index
	0.05f,						   // Animation speed
	0.0f,						   // Start rotation
	false,						   // Random rotation
	1.0f,						   // Min scale
	1.0f,						   // Max scale
	"Assets/Textures/RailGunAuto.png", // Texture path
	true						   // Is Local Space
	};

	const ParticlePreset RailGun_Semi = {
	ParticleType::RAILGUN_SEMI,
	true,						   // PlayOnAwake
	5,						   // Duration (only if one-shot)
	glm::vec3(1,1,1),   // Start color (light gray)
	glm::vec3(1,1,1),   // End color (dark gray)
	0.8f,                          // Alpha start
	1.0f,                          // Alpha end
	0.3f,                          // Size start
	0.4f,                          // Size end
	3.5f,                          // Min lifetime
	4.0f,                          // Max lifetime
	0.0f,                          // Min speed
	0.0f,                          // Max speed
	0.0f,						   // End Speed
	glm::vec3(0.0f,0.001f,1.2f),	   // Gravity (negative for upward)
	0,                          // Rotation speed
	26,                         // Emission rate (particles per second)
	EmitterShape::CONE,            // Shape
	0.1f,                          // Cone base radius
	1.0f,                          // Cone height
	20.0f,                         // Cone angle in degrees
	glm::vec2(641,361),			   // Sprite size
	true,						   // Use animation
	true,						   // Random animation Index
	0.05f,						   // Animation speed
	0.0f,						   // Start rotation
	false,						   // Random rotation
	1.0f,						   // Min scale
	1.0f,						   // Max scale
	"Assets/Textures/RailGunSemi.png", // Texture path
	true						   // Is Local Space
	};

	const ParticlePreset Environment_Dropplet = {
	ParticleType::ENVIRONMENT_DROPPLET,
	true,						   // PlayOnAwake
	5,						   // Duration (only if one-shot)
	glm::vec3(74.0 /255.0, 65.0/255.0, 42.0/255.0),   // Start color (light gray)
	glm::vec3(74.0 / 255.0, 65.0 / 255.0, 42.0 / 255.0),    // End color (dark gray)
	1.0f,                          // Alpha start
	1.0f,                          // Alpha end
	1.0f,                          // Size start
	1.0f,                          // Size end
	3.5f,                          // Min lifetime
	4.0f,                          // Max lifetime
	1.0f,                          // Min speed
	1.5f,                          // Max speed
	0.0f,						   // End Speed
	glm::vec3(0.0f,-10.0f,0.0f),	   // Gravity (negative for upward)
	0,                          // Rotation speed
	0.2f,                         // Emission rate (particles per second)
	EmitterShape::POINT,            // Shape
	0.1f,                          // Cone base radius
	1.0f,                          // Cone height
	20.0f,                         // Cone angle in degrees
	glm::vec2(641,361),			   // Sprite size
	false,						   // Use animation
	false,						   // Random animation Index
	0.05f,						   // Animation speed
	180.0f,						   // Start rotation
	false,						   // Random rotation
	1.0f,						   // Min scale
	1.5f,						   // Max scale
	"Assets/Textures/dropplet.png", // Texture path
	false						   // Is Local Space
	};

	const ParticlePreset Environment_Spark = {
	ParticleType::ENVIRONMENT_SPARK,
	true,						   // PlayOnAwake
	5,						   // Duration (only if one-shot)
	glm::vec3(1,0,1),   // Start color (light gray)
	glm::vec3(1,0,1),   // End color (dark gray)
	1.0f,                          // Alpha start
	1.0f,                          // Alpha end
	3.0f,                          // Size start
	3.0f,                          // Size end
	1.0f,                          // Min lifetime
	1.0f,                          // Max lifetime
	0.0f,                          // Min speed
	0.0f,                          // Max speed
	0.0f,						   // End Speed
	glm::vec3(0.0f,0.001f,0.0f),	   // Gravity (negative for upward)
	0,                          // Rotation speed
	1,                         // Emission rate (particles per second)
	EmitterShape::POINT,            // Shape
	0.1f,                          // Cone base radius
	1.0f,                          // Cone height
	20.0f,                         // Cone angle in degrees
	glm::vec2(144,174),			   // Sprite size
	true,						   // Use animation
	false,						   // Random animation Index
	0.05f,						   // Animation speed
	0.0f,						   // Start rotation
	false,						   // Random rotation
	1.0f,						   // Min scale
	1.0f,						   // Max scale
	"Assets/Textures/Spark.png", // Texture path
	false						   // Is Local Space
	};

	const ParticlePreset Arc_Snare_Impact = { // la que pone thundaaar
	ParticleType::ARC_SNARE_IMPACT,
	true,						   // PlayOnAwake
	2,						   // Duration (only if one-shot)
	glm::vec3(1,1,1),   // Start color (light gray)
	glm::vec3(1,1,1),   // End color (dark gray)
	1.0f,                          // Alpha start
	1.0f,                          // Alpha end
	1,                          // Size start
	10,                          // Size end
	1,                          // Min lifetime
	1,                          // Max lifetime
	0.0f,                          // Min speed
	0.0f,                          // Max speed
	0.0f,						   // End Speed
	glm::vec3(0.0f,0.001f,0.0f),	   // Gravity (negative for upward)
	0,                          // Rotation speed
	1,                         // Emission rate (particles per second)
	EmitterShape::POINT,            // Shape
	0.2f,                          // Cone base radius
	1.0f,                          // Cone height
	20.0f,                         // Cone angle in degrees
	glm::vec2(240,135),			   // Sprite size
	true,						   // Use animation
	true,						   // Random animation Index
	0.05f,						   // Animation speed
	0.0f,						   // Start rotation
	true,						   // Random rotation
	1.0f,						   // Min scale
	1.0f,						   // Max scale
	"Assets/Textures/thundaaar2.png", // Texture path
	false						   // Is Local Space
	};

	const ParticlePreset Medicae_Stim = {
		ParticleType::MEDICAE_STIM,
			true,						   // PlayOnAwake
		5,						   // Duration (only if one-shot)
		glm::vec3(1,1,1),   // Start color (light gray)
		glm::vec3(1,1,1),   // End color (dark gray)
		0.35f,                          // Alpha start
		1.0f,                          // Alpha end
		0.8f,                          // Size start
		8.0f,                          // Size end
		1,                          // Min lifetime
		1,                          // Max lifetime
		0.0f,                          // Min speed
		0.0f,                          // Max speed
		0.0f,						   // End Speed
		glm::vec3(0.0f,0.001f,0.0f),	   // Gravity (negative for upward)
		0,                          // Rotation speed
		2.6f,                         // Emission rate (particles per second)
		EmitterShape::POINT,            // Shape
		0.2f,                          // Cone base radius
		1.0f,                          // Cone height
		20.0f,                         // Cone angle in degrees
		glm::vec2(384,216),			   // Sprite size
		true,						   // Use animation
		true,						   // Random animation Index
		0.03f,						   // Animation speed
		0.0f,						   // Start rotation
		true,						   // Random rotation
		1.0f,						   // Min scale
		1.0f,						   // Max scale
		"Assets/Textures/Medicae_Stim.png", // Texture path
		false 						   // Is Local Space
	};
	const ParticlePreset Medicae_Stim_Speed = {
		ParticleType::MEDICAE_STIM_SPEED,
			false,						   // PlayOnAwake
		5,						   // Duration (only if one-shot)
		glm::vec3(1,1,1),   // Start color (light gray)
		glm::vec3(1,1,1),   // End color (dark gray)
		0.17f,                          // Alpha start
		0.17f,                          // Alpha end
		0.8f,                          // Size start
		8.0f,                          // Size end
		0.1f,                          // Min lifetime
		0.2f,                          // Max lifetime
		0.0f,                          // Min speed
		0.0f,                          // Max speed
		0.0f,						   // End Speed
		glm::vec3(0.0f,0.001f,0.0f),	   // Gravity (negative for upward)
		0,                          // Rotation speed
		80.0f,                         // Emission rate (particles per second)
		EmitterShape::CIRCLE,            // Shape
		2.11f,                          // Cone base radius
		1.0f,                          // Cone height
		20.0f,                         // Cone angle in degrees
		glm::vec2(1024,778),			   // Sprite size
		true,						   // Use animation
		true,						   // Random animation Index
		0.03f,						   // Animation speed
		0.0f,						   // Start rotation
		true,						   // Random rotation
		1.0f,						   // Min scale
		1.0f,						   // Max scale
		"Assets/Textures/Project3_Velocity_effect.png", // Texture path
		false 						   // Is Local Space
	};
	const ParticlePreset Ammunition_Blessing = {
		ParticleType::AMMUNITION_BLESSING,
			false,						   // PlayOnAwake
		5,						   // Duration (only if one-shot)
		glm::vec3(1,1,1),   // Start color (light gray)
		glm::vec3(1,1,1),   // End color (dark gray)
		1.0f,                          // Alpha start
		0.70f,                          // Alpha end
		0.8f,                          // Size start
		2.0f,                          // Size end
		0.1f,                          // Min lifetime
		1.70f,                          // Max lifetime
		0.25f,                          // Min speed
		2.0f,                          // Max speed
		0.0f,						   // End Speed
		glm::vec3(0.0f,2.601f,0.0f),	   // Gravity (negative for upward)
		0,                          // Rotation speed
		15.0f,                         // Emission rate (particles per second)
		EmitterShape::SPHERE,            // Shape
		1.84f,                          // Cone base radius
		1.0f,                          // Cone height
		20.0f,                         // Cone angle in degrees
		glm::vec2(778,1024),			   // Sprite size
		true,						   // Use animation
		false,						   // Random animation Index
		0.03f,						   // Animation speed
		180.0f,						   // Start rotation
		false,						   // Random rotation
		1.0f,						   // Min scale
		1.0f,						   // Max scale
		"Assets/Textures/Project3_AmmunitionBlessing_effect_Yiwei.png", // Texture path
		true 						   // Is Local Space
	};
	const ParticlePreset Blood_Splash = {
		ParticleType::DEFAULT,
		false,                         // PlayOnAwake
		2.0f,                          // Duration (only if one-shot)
		glm::vec3(1.0f, 1.0f, 1.0f),   // Start color (white)
		glm::vec3(1.0f, 1.0f, 1.0f),   // End color (white)
		1.0f,                          // Alpha start
		0.0f,                          // Alpha end
		1.0f,                          // Size start
		1.0f,                          // Size end
		0.5f,                          // Min lifetime
		1.0f,                          // Max lifetime
		20.0f,                         // Min speed
		0.01f,                         // Max speed
		0.01f,                         // End Speed
		glm::vec3(3.0f, 0.0f, 0.0f),   // Gravity
		0.0f,                          // Rotation speed
		10.0f,                         // Emission rate (particles per second)
		EmitterShape::POINT,           // Shape
		0.0f,                          // Unused for point
		0.0f,                          // Unused for point
		0.0f,                          // Unused for point
		glm::vec2(512.0f, 512.0f),     // Sprite size
		false,                         // Use animation
		false,                         // Random animation Index
		0.0f,                          // Animation speed
		0.0f,                          // Start rotation
		true,                          // Random rotation
		10.0f,                         // Min scale
		5.0f,                          // Max scale
		"Assets/Textures/BloodSplash_decals1_Yiwei.png", // Texture path
		false						   // Is Local Space
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
	, endSpeed(1.0f)
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

void ParticleFX::Awake() {

}

void ParticleFX::Start() {
	if (owner->GetTransform()) {
		position = owner->GetTransform()->GetPosition();
		rotation = owner->GetTransform()->GetRotation();
		scale = owner->GetTransform()->GetScale();
	}

	if (playOnAwake) {
		Play();
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
	durationTrack += deltaTime;
	while (timeSinceLastEmit >= emitInterval && isPlaying && !isPaused) {
		EmitParticle();
		timeSinceLastEmit -= emitInterval;
		if (durationTrack >= duration && isOneShot) 
		{
			Pause();
			durationTrack = 0.0f;
		}
	}

#ifndef _BUILD
	glm::vec3 cameraPosition = Application->camera->GetTransform().GetPosition();
	glm::vec3 cameraUp = Application->camera->GetTransform().GetUp();

	glm::mat4 modelMatrix = owner->GetTransform()->GetMatrix();
	glm::mat4 viewMatrix = Application->camera->view();
	glm::mat4 projMatrix = Application->camera->projection();

#else
	glm::vec3 cameraPosition = Application->root->mainCamera->GetTransform()->GetPosition();
	glm::vec3 cameraUp = Application->root->mainCamera->GetTransform()->GetUp();

	glm::mat4 modelMatrix = owner->GetTransform()->GetMatrix();
	glm::mat4 viewMatrix = Application->root->mainCamera->GetComponent<CameraComponent>()->view();
	glm::mat4 projMatrix = Application->root->mainCamera->GetComponent<CameraComponent>()->projection();
#endif // !

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
	clone->isPlaying = isPlaying;
	clone->isPaused = isPaused;
	clone->playOnAwake = playOnAwake;
	clone->duration = duration;
	clone->position = position;
	clone->rotation = rotation;
	clone->scale = scale;
	clone->spriteSize = spriteSize;
	clone->useAnimation = useAnimation;
	clone->animSpeed = animSpeed;
	clone->endSpeed = endSpeed;
	clone->randomRotation = randomRotation;
	clone->minSize = minSize;
	clone->maxSize = maxSize;
	clone->shapeParam1 = shapeParam1;
	clone->shapeParam2 = shapeParam2;
	clone->shapeParam3 = shapeParam3;
	clone->burstEmitted = burstEmitted;
	clone->durationTrack = durationTrack;
	clone->rng = rng;
	clone->dist01 = dist01;


	return clone;
}

void ParticleFX::EmitParticle() {
	ParticleData particle;

	particle.duration = duration;

	particle.maxLifetime = minLifetime + dist01(rng) * (maxLifetime - minLifetime);
	particle.lifetime = 0.0f;

	particle.parent = owner;

	particle.isLocalSpace = isLocalSpace;

	if (isLocalSpace) 
	{
		particle.position = GenerateRandomPosition();
	}
	else {
		particle.position = position + GenerateRandomPosition();
	}


	particle.velocity = GenerateRandomVelocity();

	particle.color = glm::vec4(startColor, startAlpha);

	particle.endColor = glm::vec4(endColor, endAlpha);

	particle.size = GenerateRandomSize(minSize,maxSize);

	particle.endSize = glm::vec2(endSize, endSize);

	particle.endVelocity = glm::normalize(particle.velocity) * endSpeed;

	if (randomRotation) 
	{
		particle.rotation = dist01(rng) * 360.0f;
	}
	else 
	{
		particle.rotation = glm::radians(startRotation);
	}


	particle.rotationSpeed = rotationSpeed;

	particle.randomRotation = randomRotation;

	particle.gravity = gravity;

	particle.sheetSize = glm::vec2(material->imagePtr->width(),material->imagePtr->height());
	
	particle.spriteSize = spriteSize;
	
	particle.useAnimation = useAnimation;
	
	particle.randomAnimIndex = randomAnimIndex;

	particle.animSpeed = animSpeed;

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

		return glm::rotate((glm::quat)owner->GetTransform()->GetRotation(),speed * glm::vec3(
			sin(phi) * cos(theta),
			cos(phi),
			sin(phi) * sin(theta))
		);
	}

	case EmitterShape::BOX: {
		float theta = dist01(rng) * 2.0f * glm::pi<float>();
		float phi = acos(2.0f * dist01(rng) - 1.0f);
		
		return glm::rotate((glm::quat) owner->GetTransform()->GetRotation(), speed * glm::vec3(
			sin(phi) * cos(theta),
			sin(phi) * sin(theta),
			cos(phi))
		);
	}

	case EmitterShape::CIRCLE: {
		return glm::rotate((glm::quat)owner->GetTransform()->GetRotation(), speed * glm::vec3(0.0f, 1.0f, 0.0f));
	}
	}

	return {0.0f, speed, 0.0f};
}

std::shared_ptr<Image> ParticleFX::GetTexture(const std::string& texturePath) {
	auto it = Application->loadedPartTextures.find(texturePath);
	if (it != Application->loadedPartTextures.end()) {
		return it->second;
	}
	return nullptr;
}

glm::vec2 ParticleFX::GenerateRandomSize(float minSize, float maxSize) {
	
	float tSize = minSize + (maxSize - minSize) * dist01(rng);
	return glm::vec2(tSize, tSize);
}

void ParticleFX::ApplyPreset(int particleID) {
	ParticleType particleType = static_cast<ParticleType>(particleID);
	auto preset = ParticlePresets::Smoke;
	switch (particleType)
	{
	case ParticleType::DEFAULT:
		preset = ParticlePresets::Smoke;
		SetOneShot(false);
		break;
	case ParticleType::SMOKE:
		preset = ParticlePresets::Smoke;
		SetOneShot(false);
		break;
	case ParticleType::FIRE:
		preset = ParticlePresets::Fire;
		SetOneShot(false);
		break;
	case ParticleType::MUZZLE_FLASH:
		preset = ParticlePresets::MuzzleFlash;
		SetOneShot(true);
		break;
	case ParticleType::EXPLOSION:
		preset = ParticlePresets::Explosion;
		SetOneShot(false);
		break;
	case ParticleType::ENVIRONMENT_SMOKE:
		preset = ParticlePresets::Environment_Smoke;
		SetOneShot(false);
		break;
	case ParticleType::FLAME:
		preset = ParticlePresets::Flame;
		SetOneShot(false);
		break;
	case ParticleType::ENVIRONMENT_EXPLOSION:
		preset = ParticlePresets::Environment_Explosion;
		SetOneShot(false);
		break;
	case ParticleType::RIFFLE_SHOT:
		preset = ParticlePresets::Riffle_Shot;
		break;
	case ParticleType::SHOTGUN_SHOT:
		preset = ParticlePresets::Shotgun_Shot;
		break;
	case ParticleType::ENEMY_DASH:
		preset = ParticlePresets::Enemy_Dash;
		SetOneShot(true);
		break;
	case ParticleType::ACID_ATK:
		preset = ParticlePresets::Acid_Atk;
		SetOneShot(true);
		break;
	case ParticleType::ACID_PUDDLE:
		preset = ParticlePresets::Acid_Puddle;
		SetOneShot(false);
		break;
	case ParticleType::ENERGY_BALL:
		preset = ParticlePresets::Energy_Ball;
		SetOneShot(false);
		break;
	case ParticleType::RAILGUN_AUTO:
		preset = ParticlePresets::RailGun_Auto;
		SetOneShot(false);
		break;
	case ParticleType::RAILGUN_SEMI:
		preset = ParticlePresets::RailGun_Semi;
		SetOneShot(false);
		break;
	case ParticleType::ENVIRONMENT_DROPPLET:
		preset = ParticlePresets::Environment_Dropplet;
		SetOneShot(false);
		break;
	case ParticleType::ENVIRONMENT_SPARK:
		preset = ParticlePresets::Environment_Spark;
		SetOneShot(false);
		break;
	case ParticleType::ARC_SNARE_IMPACT:
		preset = ParticlePresets::Arc_Snare_Impact;
		SetOneShot(true);
		break;
	case ParticleType::BLOOD_SPLASH:
		preset = ParticlePresets::Blood_Splash;
		SetOneShot(true);
		break;
	case ParticleType::MEDICAE_STIM_SPEED:
		preset = ParticlePresets::Medicae_Stim_Speed;
		SetOneShot(false);
		break;
	case ParticleType::AMMUNITION_BLESSING:
		preset = ParticlePresets::Ammunition_Blessing;
		SetOneShot(false);
		break;
	default:
		preset = ParticlePresets::Medicae_Stim;
		SetOneShot(false);
		break;
	}

	material->SetParticleType(preset.type);
	playOnAwake = preset.playOnAwake;
	duration = preset.duration;
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
	spriteSize = preset.spriteSize;
	useAnimation = preset.useAnimation;
	animSpeed = preset.animSpeed;
	endSpeed = preset.endSpeed;
	startRotation = preset.startRotation;
	randomRotation = preset.randomRotation;
	minSize = preset.minSize;
	maxSize = preset.maxSize;
	randomAnimIndex = preset.randomAnimIndex;
	isLocalSpace = preset.isLocalSpace;
	//SetTexture(preset.texturePath);

	// Asignar textura desde el mapa
	auto texture = GetTexture(preset.texturePath);
	if (texture) {
		material->setImage(texture);
	}
	else {
		std::cerr << "Texture not found: " << preset.texturePath << std::endl;
	}
}

void ParticleFX::SetEndSpeed(float Espeed) 
{
	this->endSpeed = Espeed;

}

void ParticleFX::EmitBurst(int count) {
	for (int i = 0; i < count; ++i) {
		EmitParticle();
	}
}

void ParticleFX::Play() {
	isPlaying = true;
	isPaused = false;
	durationTrack = 0;

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
	burstEmitted = false;
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

void ParticleFX::SetParticleEndSize(float endSize) 
{
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

void ParticleFX::SetGravity(glm::vec3 gravity) {
	this->gravity = gravity;
}

void ParticleFX::SetTexture(const std::string& texturePath) {
	auto image = std::make_shared<Image>();
	std::string binaryPath = texturePath.substr(texturePath.find_last_of('/') + 1);
	binaryPath = binaryPath.substr(0, binaryPath.size() - 4);
	auto binImage = image->LoadBinary(binaryPath);
	if (binImage == NULL)
	{
		image->LoadTexture(texturePath);
		
		image->SaveBinary(binaryPath);
		material->setImage(image);
	}
	else
	{
		material->setImage(binImage);
	}

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

MonoObject* ParticleFX::GetSharp()
{
	if (CsharpReference) {
		return CsharpReference;
	}
	MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "ParticleFX");
	if (!klass) {
		return nullptr;
	}
	MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
	if (!monoObject) {
		return nullptr;
	}
	MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.ParticleFX:.ctor(uintptr,HawkEngine.GameObject)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
	if (!method)
	{
		return nullptr;
	}
	uintptr_t componentPtr = reinterpret_cast<uintptr_t>(this);
	MonoObject* ownerGo = owner ? owner->GetSharp() : nullptr;
	if (!ownerGo)
	{
		return nullptr;
	}
	void* args[2];
	args[0] = &componentPtr;
	args[1] = ownerGo;
	mono_runtime_invoke(method, monoObject, args, NULL);
	CsharpReference = monoObject;
	return CsharpReference;
}