//#include "SoundComponent.h"
//#include <MyScriptingEngine/MonoManager.h>
//#include <mono/metadata/debug-helpers.h>
//
//SoundComponent::SoundComponent(GameObject* owner, AudioEngine* audioEngine)
//	: Component(owner)
//{
//	this->audioEngine = audioEngine;
//	name = "SoundComponent";
//	updateInStop = false;
//}
//
//SoundComponent::~SoundComponent()
//{
//	Destroy();
//}
//
//std::unique_ptr<Component> SoundComponent::Clone(GameObject* new_owner) {
//	return std::make_unique<SoundComponent>(new_owner, audioEngine);
//}
//
//void SoundComponent::Awake()
//{
//	// Initialize the audio engine or any other setup needed
//	//audioEngine->Init();
//}
//
//void SoundComponent::Start()
//{
//
//}
//
//void SoundComponent::Update(float deltaTime)
//{
//	// Update the audio engine
//	// No hacer el update aqui porque si no se hara un update por cada sound component
//	//audioEngine->Update();
//}
//
//void SoundComponent::Destroy()
//{
//	// Clean up any resources
//	// No hacer shutdown del audio engine aqui, ya que lo rompera para todos los componentes
//	//audioEngine->Shutdown();
//}
//
//void SoundComponent::LoadSound(const std::string& soundFile, bool is3D, bool loop)
//{
//	audioEngine->LoadSound(soundFile, is3D, loop);
//}
//
//void SoundComponent::PlaySound(const std::string& soundName)
//{
//	audioEngine->PlaySound(soundName);
//}
//
//void SoundComponent::StopSound(const std::string& soundName)
//{
//	int channelId = audioEngine->GetChannelId(soundName);
//	audioEngine->StopSound(channelId);
//}
//
//void SoundComponent::PauseSound(const std::string& soundName)
//{
//	int channelId = audioEngine->GetChannelId(soundName);
//	audioEngine->PauseSound(channelId);
//}
//
//void SoundComponent::ResumeSound(const std::string& soundName)
//{
//	int channelId = audioEngine->GetChannelId(soundName);
//	audioEngine->ResumeSound(channelId);
//}
//
//void SoundComponent::SetVolume(const std::string& soundName, float volume)
//{
//	int channelId = audioEngine->GetChannelId(soundName);
//	audioEngine->SetChannelVolume(channelId, volume);
//}
//
//int SoundComponent::GetChannelId(const std::string& soundName)
//{
//	return audioEngine->GetChannelId(soundName);
//}
//
//MonoObject* SoundComponent::GetSharp()
//{
//	MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "AudioSource");
//	if (!klass) {
//		return nullptr;
//	}
//
//	MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
//	if (!monoObject) {
//		return nullptr;
//	}
//
//	MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.AudioSource:.ctor(uintptr,HawkEngine.GameObject)", true);
//	MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
//	if (!method)
//	{
//		return nullptr;
//	}
//
//	uintptr_t componentPtr = reinterpret_cast<uintptr_t>(this);
//	MonoObject* ownerGo = owner->GetSharp();
//	if (!ownerGo)
//	{
//		return nullptr;
//	}
//
//	void* args[2];
//	args[0] = &componentPtr;
//	args[1] = ownerGo;
//
//	mono_runtime_invoke(method, monoObject, args, NULL);
//
//	return monoObject;
//}
//
