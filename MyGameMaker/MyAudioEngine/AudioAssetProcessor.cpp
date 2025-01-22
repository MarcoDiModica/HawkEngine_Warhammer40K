#include "AudioAssetProcessor.h"
#include "../MyGameEditor/Log.h"
#include <filesystem>
#include <fstream>

namespace MyGameEngine {

std::unordered_map<std::string, std::weak_ptr<AudioAsset>> AudioAssetProcessor::audioCache;

bool AudioAssetProcessor::ProcessAudioFile(const std::string& sourcePath, const std::string& outputPath) {
    if (!IsAudioFile(sourcePath)) {
        LOG(LogType::LOG_ERROR, "Not a valid audio file: %s", sourcePath.c_str());
        return false;
    }

    // Create output directory if it doesn't exist
    std::filesystem::path outputDir = std::filesystem::path(outputPath).parent_path();
    if (!CreateDirectoryIfNotExists(outputDir.string())) {
        LOG(LogType::LOG_ERROR, "Failed to create output directory: %s", outputDir.string().c_str());
        return false;
    }

    try {
        // For now, we'll just copy the file - in the future we can add format conversion
        std::filesystem::copy_file(sourcePath, outputPath, 
            std::filesystem::copy_options::overwrite_existing);
        LOG(LogType::LOG_OK, "Processed audio file: %s -> %s", sourcePath.c_str(), outputPath.c_str());
        return true;
    }
    catch (const std::exception& e) {
        LOG(LogType::LOG_ERROR, "Failed to process audio file: %s", e.what());
        return false;
    }
}

std::shared_ptr<AudioAsset> AudioAssetProcessor::LoadProcessedAudio(const std::string& libraryPath) {
    // Check cache first
    if (auto asset = GetCachedAsset(libraryPath)) {
        return asset;
    }

    // Create new asset
    auto asset = std::make_shared<AudioAsset>();
    if (!asset->LoadFromFile(libraryPath)) {
        LOG(LogType::LOG_ERROR, "Failed to load processed audio: %s", libraryPath.c_str());
        return nullptr;
    }

    // Add to cache
    audioCache[libraryPath] = asset;
    return asset;
}

std::shared_ptr<AudioAsset> AudioAssetProcessor::GetCachedAsset(const std::string& path) {
    auto it = audioCache.find(path);
    if (it != audioCache.end()) {
        if (auto asset = it->second.lock()) {
            return asset;
        }
        // Remove expired weak pointer
        audioCache.erase(it);
    }
    return nullptr;
}

void AudioAssetProcessor::ClearCache() {
    audioCache.clear();
    LOG(LogType::LOG_INFO, "Audio asset cache cleared");
}

void AudioAssetProcessor::UnloadUnused() {
    for (auto it = audioCache.begin(); it != audioCache.end();) {
        if (it->second.expired()) {
            it = audioCache.erase(it);
        } else {
            ++it;
        }
    }
    LOG(LogType::LOG_INFO, "Unused audio assets unloaded");
}

bool AudioAssetProcessor::CreateDirectoryIfNotExists(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        return std::filesystem::create_directories(path);
    }
    return true;
}

std::string AudioAssetProcessor::GetFileExtension(const std::string& path) {
    std::string extension = std::filesystem::path(path).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension;
}

bool AudioAssetProcessor::IsAudioFile(const std::string& path) {
    std::string ext = GetFileExtension(path);
    return ext == ".wav" || ext == ".ogg" || ext == ".mp3";
}

} 