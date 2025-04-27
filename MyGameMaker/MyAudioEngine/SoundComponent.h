//#pragma once
//#include "../MyGameEngine/Component.h"
//#include "AudioEngine.h"
//#include <string>
//#include <memory>
//
//
//class SoundComponent : public Component
//{
//public:
//	SoundComponent(GameObject* owner, AudioEngine* audioEngine);
//	~SoundComponent();
//
//	void Awake() override;
//
//	void Start() override;
//
//	void Update(float deltaTime) override;
//
//	void Destroy() override;
//
//	ComponentType GetType() const override {
//		return ComponentType::AUDIO; // Cambia a un tipo espec?fico si es necesario
//	}
//
//	std::unique_ptr<Component> Clone(GameObject* new_owner) override;
//
//	AudioEngine* audioEngine = nullptr;
//
//public:
//
//	void LoadSound(const std::string& soundFile, bool is3D, bool loop);
//	void PlaySound(const std::string& soundName);
//	void StopSound(const std::string& soundName);
//	void PauseSound(const std::string& soundName);
//	void ResumeSound(const std::string& soundName);
//	void SetVolume(const std::string& soundName, float volume);
//	int GetChannelId(const std::string& soundName);
//
//	MonoObject* CsharpReference = nullptr;
//	MonoObject* GetSharp() override;
//};
//
