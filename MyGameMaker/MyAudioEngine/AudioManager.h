#pragma once
#include "AudioEngine.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <functional>

class AudioManager {
private:
	static AudioEngine* audioEngine;
	static std::unordered_map<std::string, int> activeAudio;
	static std::unordered_map<int, std::string> idToPath;
	static std::unordered_map<int, bool> isMusicTrack;
	static std::vector<int> musicChannels;
	static std::vector<int> sfxChannels;

	static float masterVolume;
	static float musicVolume;
	static float sfxVolume;

	struct ScheduledAudio {
		std::string path;
		bool loop;
		float delay;
		float timeRemaining;
	};

	static std::vector<ScheduledAudio> scheduledAudio;

public:
	static void Initialize();
	static void Shutdown();
	static void Update(float deltaTime);

	static int Play(const std::string& path, bool loop = false);
	static int PlayOneShot(const std::string& path);
	static void Stop(int audioId);
	static void Stop(const std::string& path);
	static void Pause(int audioId);
	static void Pause(const std::string& path);
	static void Resume(int audioId);
	static void Resume(const std::string& path);

	static int PlayMusic(const std::string& path);
	static void StopMusic(const std::string& path);
	static void StopAllMusic();

	static void SetMasterVolume(float volume);
	static void SetMusicVolume(float volume);
	static void SetSfxVolume(float volume);
	static void SetVolume(int audioId, float volume);
	static void SetVolume(const std::string& path, float volume);

	static float GetMasterVolume();
	static float GetMusicVolume();
	static float GetSfxVolume();

	static void StopAll();
	static void PauseAll();
	static void ResumeAll();

	static void SchedulePlay(const std::string& path, float delay, bool loop = false);

	static int GetChannelId(const std::string& path);
	static void UpdateVolumes();
};