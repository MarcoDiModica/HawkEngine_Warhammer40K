#include "AudioManager.h"

AudioEngine* AudioManager::audioEngine = nullptr;
std::unordered_map<std::string, int> AudioManager::activeAudio;
std::unordered_map<int, std::string> AudioManager::idToPath;
std::unordered_map<int, bool> AudioManager::isMusicTrack;
std::vector<int> AudioManager::musicChannels;
std::vector<int> AudioManager::sfxChannels;
std::vector<AudioManager::ScheduledAudio> AudioManager::scheduledAudio;

float AudioManager::masterVolume = 1.0f;
float AudioManager::musicVolume = 1.0f;
float AudioManager::sfxVolume = 1.0f;

void AudioManager::Initialize() {
	if (!audioEngine) {
		audioEngine = new AudioEngine();
		AudioEngine::Init();
	}
}

void AudioManager::Shutdown() {
	StopAll();
	audioEngine->Shutdown();
	delete audioEngine;
	audioEngine = nullptr;

	activeAudio.clear();
	idToPath.clear();
	isMusicTrack.clear();
	musicChannels.clear();
	sfxChannels.clear();
	scheduledAudio.clear();
}

void AudioManager::Update(float deltaTime) {
	AudioEngine::Update();

	for (auto it = scheduledAudio.begin(); it != scheduledAudio.end();) {
		it->timeRemaining -= deltaTime;

		if (it->timeRemaining <= 0) {
			Play(it->path, it->loop);
			it = scheduledAudio.erase(it);
		}
		else {
			++it;
		}
	}

	std::vector<int> channelsToRemove;

	for (const auto& pair : activeAudio) {
		int channelId = pair.second;
		if (!audioEngine->IsPlaying(channelId)) {
			channelsToRemove.push_back(channelId);
		}
	}

	for (int channelId : channelsToRemove) {
		std::string path = idToPath[channelId];
		activeAudio.erase(path);
		idToPath.erase(channelId);

		auto it = std::find(musicChannels.begin(), musicChannels.end(), channelId);
		if (it != musicChannels.end()) {
			musicChannels.erase(it);
		}

		it = std::find(sfxChannels.begin(), sfxChannels.end(), channelId);
		if (it != sfxChannels.end()) {
			sfxChannels.erase(it);
		}
	}
}

int AudioManager::Play(const std::string& path, bool loop) {
	if (!audioEngine) return -1;

	audioEngine->LoadSound(path, false, loop);

	int channelId = audioEngine->PlaySound(path);

	activeAudio[path] = channelId;
	idToPath[channelId] = path;

	isMusicTrack[channelId] = false;
	sfxChannels.push_back(channelId);

	audioEngine->SetChannelVolume(channelId, sfxVolume * masterVolume);

	return channelId;
}

int AudioManager::PlayOneShot(const std::string& path) {
	return Play(path, false);
}

void AudioManager::Stop(int audioId) {
	if (!audioEngine || audioId < 0) return;

	audioEngine->StopSound(audioId);

	if (idToPath.find(audioId) != idToPath.end()) {
		std::string path = idToPath[audioId];
		activeAudio.erase(path);
		idToPath.erase(audioId);

		auto it = std::find(musicChannels.begin(), musicChannels.end(), audioId);
		if (it != musicChannels.end()) {
			musicChannels.erase(it);
		}

		it = std::find(sfxChannels.begin(), sfxChannels.end(), audioId);
		if (it != sfxChannels.end()) {
			sfxChannels.erase(it);
		}
	}
}

void AudioManager::Stop(const std::string& path) {
	int channelId = GetChannelId(path);
	if (channelId >= 0) {
		Stop(channelId);
	}
}

void AudioManager::Pause(int audioId) {
	if (!audioEngine || audioId < 0) return;
	audioEngine->PauseSound(audioId);
}

void AudioManager::Pause(const std::string& path) {
	int channelId = GetChannelId(path);
	if (channelId >= 0) {
		Pause(channelId);
	}
}

void AudioManager::Resume(int audioId) {
	if (!audioEngine || audioId < 0) return;
	audioEngine->ResumeSound(audioId);
}

void AudioManager::Resume(const std::string& path) {
	int channelId = GetChannelId(path);
	if (channelId >= 0) {
		Resume(channelId);
	}
}

int AudioManager::PlayMusic(const std::string& path) {
	if (!audioEngine) return -1;

	audioEngine->LoadSound(path, false, true);

	int channelId = audioEngine->PlaySound(path);

	activeAudio[path] = channelId;
	idToPath[channelId] = path;

	isMusicTrack[channelId] = true;
	musicChannels.push_back(channelId);

	audioEngine->SetChannelVolume(channelId, musicVolume * masterVolume);

	return channelId;
}

void AudioManager::StopMusic(const std::string& path) {
	int channelId = GetChannelId(path);
	if (channelId >= 0 && isMusicTrack[channelId]) {
		Stop(channelId);
	}
}

void AudioManager::StopAllMusic() {
	for (auto channelId : musicChannels) {
		audioEngine->StopSound(channelId);
	}

	for (int channelId : musicChannels) {
		std::string path = idToPath[channelId];
		activeAudio.erase(path);
		idToPath.erase(channelId);
	}

	musicChannels.clear();
}

void AudioManager::SetMasterVolume(float volume) {
	masterVolume = volume;
	UpdateVolumes();
}

void AudioManager::SetMusicVolume(float volume) {
	musicVolume = volume;

	for (int channelId : musicChannels) {
		audioEngine->SetChannelVolume(channelId, musicVolume * masterVolume);
	}
}

void AudioManager::SetSfxVolume(float volume) {
	sfxVolume = volume;

	for (int channelId : sfxChannels) {
		audioEngine->SetChannelVolume(channelId, sfxVolume * masterVolume);
	}
}

void AudioManager::SetVolume(int audioId, float volume) {
	if (!audioEngine || audioId < 0) return;

	float scaledVolume = volume;
	if (isMusicTrack[audioId]) {
		scaledVolume *= musicVolume * masterVolume;
	}
	else {
		scaledVolume *= sfxVolume * masterVolume;
	}

	audioEngine->SetChannelVolume(audioId, scaledVolume);
}

void AudioManager::SetVolume(const std::string& path, float volume) {
	int channelId = GetChannelId(path);
	if (channelId >= 0) {
		SetVolume(channelId, volume);
	}
}

float AudioManager::GetMasterVolume() {
	return masterVolume;
}

float AudioManager::GetMusicVolume() {
	return musicVolume;
}

float AudioManager::GetSfxVolume() {
	return sfxVolume;
}

void AudioManager::StopAll() {
	if (!audioEngine) return;

	audioEngine->StopAllChannels();

	activeAudio.clear();
	idToPath.clear();
	isMusicTrack.clear();
	musicChannels.clear();
	sfxChannels.clear();
}

void AudioManager::PauseAll() {
	if (!audioEngine) return;
	audioEngine->PauseAllChannels();
}

void AudioManager::ResumeAll() {
	if (!audioEngine) return;
	audioEngine->ResumeAllChannels();
}

void AudioManager::SchedulePlay(const std::string& path, float delay, bool loop) {
	ScheduledAudio scheduled;
	scheduled.path = path;
	scheduled.loop = loop;
	scheduled.delay = delay;
	scheduled.timeRemaining = delay;

	scheduledAudio.push_back(scheduled);
}

int AudioManager::GetChannelId(const std::string& path) {
	auto it = activeAudio.find(path);
	if (it != activeAudio.end()) {
		return it->second;
	}
	return -1;
}

void AudioManager::UpdateVolumes() {
	for (int channelId : musicChannels) {
		audioEngine->SetChannelVolume(channelId, musicVolume * masterVolume);
	}

	for (int channelId : sfxChannels) {
		audioEngine->SetChannelVolume(channelId, sfxVolume * masterVolume);
	}
}