#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "AudioAsset.h"

namespace MyGameEngine {

class AudioAssetProcessor {
public:
    // Process audio file into engine-friendly format
    static bool ProcessAudioFile(const std::string& sourcePath, const std::string& outputPath);
    
    // Load processed audio file
    static std::shared_ptr<AudioAsset> LoadProcessedAudio(const std::string& libraryPath);
    
    // Cache management
    static void ClearCache();
    static void UnloadUnused();

    // Get cached asset if it exists
    static std::shared_ptr<AudioAsset> GetCachedAsset(const std::string& path);

private:
    static std::unordered_map<std::string, std::weak_ptr<AudioAsset>> audioCache;
    
    // Helper functions
    static bool CreateDirectoryIfNotExists(const std::string& path);
    static std::string GetFileExtension(const std::string& path);
    static bool IsAudioFile(const std::string& path);
};

} 