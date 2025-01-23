#include "AudioAssetProcessor.h"
#include <filesystem>
#include <fstream>

namespace MyGameEngine {

std::unordered_map<std::string, std::weak_ptr<AudioAsset>> AudioAssetProcessor::audioCache;

bool AudioAssetProcessor::ProcessAudioFile(const std::string& sourcePath, const std::string& outputPath) {
    if (!IsAudioFile(sourcePath)) {
        return false;
    }

    // Create output directory if it doesn't exist
    std::filesystem::path outputDir = std::filesystem::path(outputPath).parent_path();
    if (!CreateDirectoryIfNotExists(outputDir.string())) {
        return false;
    }

    try {
        // For now, we'll just copy the file - in the future we can add format conversion
        std::filesystem::copy_file(sourcePath, outputPath, 
            std::filesystem::copy_options::overwrite_existing);
        return true;
    }
    catch (const std::exception& e) {
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
}

void AudioAssetProcessor::UnloadUnused() {
    for (auto it = audioCache.begin(); it != audioCache.end();) {
        if (it->second.expired()) {
            it = audioCache.erase(it);
        } else {
            ++it;
        }
    }
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