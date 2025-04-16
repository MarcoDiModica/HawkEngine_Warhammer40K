#include "SoundComponent.h"

SoundComponent::SoundComponent(GameObject* owner, AudioEngine* audioEngine)
	: Component(owner)
{
	this->audioEngine = audioEngine;
	name = "SoundComponent";
	updateInStop = false;
}

SoundComponent::~SoundComponent()
{
	Destroy();
}

void SoundComponent::Awake()
{
	// Initialize the audio engine or any other setup needed
	//audioEngine->Init();
}

void SoundComponent::Start()
{

}

void SoundComponent::Update(float deltaTime)
{
	// Update the audio engine
	audioEngine->Update();
}

void SoundComponent::Destroy()
{
	// Clean up any resources
	audioEngine->Shutdown();
}

void SoundComponent::LoadSound(const std::string& soundFile, bool is3D, bool loop)
{
	audioEngine->LoadSound(soundFile, is3D, loop);
}

void SoundComponent::PlaySound(const std::string& soundName)
{
	audioEngine->PlaySound(soundName);
}

void SoundComponent::StopSound(const std::string& soundName)
{
	int channelId = audioEngine->GetChannelId(soundName);
	audioEngine->StopSound(channelId);
}

void SoundComponent::PauseSound(const std::string& soundName)
{
	int channelId = audioEngine->GetChannelId(soundName);
	audioEngine->PauseSound(channelId);
}

void SoundComponent::ResumeSound(const std::string& soundName)
{
	int channelId = audioEngine->GetChannelId(soundName);
	audioEngine->ResumeSound(channelId);
}

void SoundComponent::SetVolume(const std::string& soundName, float volume)
{
	int channelId = audioEngine->GetChannelId(soundName);
	audioEngine->SetChannelVolume(channelId, volume);
}

int SoundComponent::GetChannelId(const std::string& soundName)
{
	return audioEngine->GetChannelId(soundName);
}

