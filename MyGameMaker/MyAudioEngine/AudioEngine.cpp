#include "AudioEngine.h"


Implementation::Implementation() {
    mpStudioSystem = NULL;
    FMOD::Studio::System::create(&mpStudioSystem);
    mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL);

    mpSystem = NULL;
	mpStudioSystem->getCoreSystem(&mpSystem);
}

Implementation::~Implementation() {
    mpStudioSystem->unloadAll();
    mpStudioSystem->release();
}

AudioEngine::AudioEngine() {

}

AudioEngine::~AudioEngine() {
	Shutdown();
}

void Implementation::Update() {
    std::vector<ChannelMap::iterator> pStoppedChannels;
    for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
    {
        bool bIsPlaying = false;
        it->second->isPlaying(&bIsPlaying);

        if (!bIsPlaying)
        {
            pStoppedChannels.push_back(it);
        }
    }
    for (auto& it : pStoppedChannels)
    {
        mChannels.erase(it);
    }
    
    mpStudioSystem->update();
}

Implementation* sgpImplementation = nullptr;

void AudioEngine::Init() {
    sgpImplementation = new Implementation;
}

void AudioEngine::Update() {
	sgpImplementation->Update();
}

void AudioEngine::Shutdown() {
	delete sgpImplementation;
	sgpImplementation = nullptr;
}

void AudioEngine::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream) {
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt != sgpImplementation->mSounds.end())
        return;

    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3d ? FMOD_3D : FMOD_2D;
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD::Sound* pSound = nullptr;
    sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound);
    if (pSound) {
        sgpImplementation->mSounds[strSoundName] = pSound;
    }
}

void AudioEngine::UnLoadSound(const std::string& strSoundName) {
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
        return;

    tFoundIt->second->release();
    sgpImplementation->mSounds.erase(tFoundIt);
}

int AudioEngine::PlaySound(const std::string& strSoundName, const glm::vec3& vPos, float fVolumedB)
{
    int nChannelId = sgpImplementation->mnNextChannelId++;
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
    {
        LoadSound(strSoundName);
        tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundIt == sgpImplementation->mSounds.end())
        {
            return nChannelId;
        }
    }
    FMOD::Channel* pChannel = nullptr;
    sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel);
    if (pChannel)
    {
        FMOD_MODE currMode;
        tFoundIt->second->getMode(&currMode);
        if (currMode & FMOD_3D) {
            FMOD_VECTOR position = VectorToFmod(vPos);
            pChannel->set3DAttributes(&position, nullptr);
        }

        float baseVolume = dbToVolume(fVolumedB);
        sgpImplementation->mChannelBaseVolumes[nChannelId] = baseVolume;

        pChannel->setVolume(baseVolume * sgpImplementation->masterVolume);
        pChannel->setPaused(false);
        sgpImplementation->mChannels[nChannelId] = pChannel;
    }
    return nChannelId;
}


void AudioEngine::StopSound(int nChannelId) {
	auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
	if (tFoundIt == sgpImplementation->mChannels.end())
		return;

	if (tFoundIt->second) {
		tFoundIt->second->stop();
		sgpImplementation->mChannels.erase(tFoundIt);
		sgpImplementation->mChannelBaseVolumes.erase(nChannelId);
	}
}

void AudioEngine::PauseSound(int nChannelId) {
	auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
	if (tFoundIt == sgpImplementation->mChannels.end())
		return;

	tFoundIt->second->setPaused(true);
}

void AudioEngine::ResumeSound(int nChannelId) {
	auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
	if (tFoundIt == sgpImplementation->mChannels.end())
		return;

	tFoundIt->second->setPaused(false);
}

void AudioEngine::StopAllChannels() {
	for (auto& channel : sgpImplementation->mChannels) {
		channel.second->stop();
	}
	sgpImplementation->mChannels.clear();
}

void AudioEngine::PauseAllChannels() {
	for (auto& channel : sgpImplementation->mChannels) {
		channel.second->setPaused(true);
	}
}

void AudioEngine::ResumeAllChannels() {
	for (auto& channel : sgpImplementation->mChannels) {
		channel.second->setPaused(false);
	}
}

int AudioEngine::GetChannelId(const std::string& strSoundName) {
	auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
	if (tFoundIt == sgpImplementation->mSounds.end())
		return -1;

	for (auto& channel : sgpImplementation->mChannels) {
		FMOD::Sound* pSound = nullptr;
		channel.second->getCurrentSound(&pSound);
		if (pSound == tFoundIt->second) {
			return channel.first;
		}
	}
	return -1;
}

void AudioEngine::SetChannel3dPosition(int nChannelId, const glm::vec3& vPosition)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return;

    FMOD_VECTOR position = VectorToFmod(vPosition);
    tFoundIt->second->set3DAttributes(&position, NULL);
}

void AudioEngine::SetChannelVolume(int nChannelId, float fVolumedB)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return;

    tFoundIt->second->setVolume(dbToVolume(fVolumedB));
}

void AudioEngine::SetMasterVolume(float fVolumedB)
{
    sgpImplementation->masterVolume = fVolumedB; // Convert dB to linear scale
    for (auto& channel : sgpImplementation->mChannels) {
        int channelId = channel.first;
        FMOD::Channel* pChannel = channel.second;

        // Retrieve the base volume for the channel
        float baseVolume = sgpImplementation->mChannelBaseVolumes[channelId];

        // Apply the master volume scaling
        pChannel->setVolume(baseVolume * sgpImplementation->masterVolume);
    }
}


bool AudioEngine::IsPlaying(int nChannelId) const
{
	auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
	if (tFoundIt == sgpImplementation->mChannels.end())
		return false;

	bool bIsPlaying = false;
	tFoundIt->second->isPlaying(&bIsPlaying);
	return bIsPlaying;
}

void AudioEngine::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
    auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
    if (tFoundIt != sgpImplementation->mBanks.end())
        return;
    FMOD::Studio::Bank* pBank;
    sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank);
    if (pBank) {
        sgpImplementation->mBanks[strBankName] = pBank;
    }
}

void AudioEngine::LoadEvent(const std::string& strEventName) {
    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
    if (tFoundit != sgpImplementation->mEvents.end())
        return;
    FMOD::Studio::EventDescription* pEventDescription = NULL;
    sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription);
    if (pEventDescription) {
        FMOD::Studio::EventInstance* pEventInstance = NULL;
        pEventDescription->createInstance(&pEventInstance);
        if (pEventInstance) {
            sgpImplementation->mEvents[strEventName] = pEventInstance;
        }
    }
}

void AudioEngine::PlayEvent(const std::string& strEventName) {
    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
    if (tFoundit == sgpImplementation->mEvents.end()) {
        LoadEvent(strEventName);
        tFoundit = sgpImplementation->mEvents.find(strEventName);
        if (tFoundit == sgpImplementation->mEvents.end())
            return;
    }
    tFoundit->second->start();
}

void AudioEngine::StopEvent(const std::string& strEventName, bool bImmediate) {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;

    FMOD_STUDIO_STOP_MODE eMode;
    eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    tFoundIt->second->stop(eMode);
}

bool AudioEngine::IsEventPlaying(const std::string& strEventName) const {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return false;

    FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
    if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
        return true;
    }
    return false;
}

void AudioEngine::GetEventParameter(const std::string& strEventName, const std::string& strParameterName, float* parameter) {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;

	tFoundIt->second->getParameterByName(strParameterName.c_str(), parameter, NULL);
}

void AudioEngine::SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue) {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;

    
	tFoundIt->second->setParameterByName(strParameterName.c_str(), fValue);
}

FMOD_VECTOR AudioEngine::VectorToFmod(const glm::vec3& vPosition) {
    FMOD_VECTOR fVec;
    fVec.x = vPosition.x;
    fVec.y = vPosition.y;
    fVec.z = vPosition.z;
    return fVec;
}

float  AudioEngine::dbToVolume(float dB)
{
    return powf(10.0f, 0.05f * dB);
}

float  AudioEngine::VolumeTodb(float volume)
{
    return 20.0f * log10f(volume);
}
