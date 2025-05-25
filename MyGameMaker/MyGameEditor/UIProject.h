#pragma once

#include <filesystem>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <future>
#include <mutex>
#include <atomic>
#include <memory>
#include <string>
#include <chrono>
#include <queue>

#include "UIElement.h"

class Image;

class UIProject : public UIElement {
public:
	enum class SortOption {
		Name,
		Type,
		Size,
		LastModified
	};

	struct ProjectSettings {
		float iconSize = 64.0f;
		float minIconSize = 32.0f;
		float maxIconSize = 128.0f;
		int thumbnailCacheSize = 100;
		int maxTexturesPerFrame = 2;
		bool showHiddenFiles = false;
		bool showThumbnails = true;
	};

	UIProject(UIType type, std::string name);
	~UIProject();

	bool Draw() override;

private:
	std::filesystem::path directoryPath;
	std::filesystem::path assetsPath;
	std::filesystem::path selectedDirectory;
	std::filesystem::path draggedItemPath;
	std::filesystem::path copiedItemPath;
	std::filesystem::path selectedFile;
	std::filesystem::path renamePath;
	std::filesystem::path newItemPath;

	std::string currentSceneFile;

	std::vector<std::filesystem::path> currentDirectoryEntries;
	std::future<std::vector<std::filesystem::path>> directoryListingFuture;
	mutable std::recursive_mutex entriesMutex;

	std::unordered_map<std::string, Image*> iconCache;
	mutable std::mutex iconCacheMutex;

	struct ThumbnailEntry {
		Image* image;
		std::chrono::steady_clock::time_point lastAccess;
	};
	std::unordered_map<std::filesystem::path, ThumbnailEntry> imagePreviewCache;
	std::unordered_set<std::filesystem::path> pendingThumbnails;
	mutable std::mutex thumbnailMutex;
	std::atomic<bool> processingThumbnails{ false };

	SortOption currentSortOption;
	bool sortAscending;
	bool firstDraw = true;
	std::atomic<bool> isLoading{ false };
	bool showDeletePopup = false;
	bool isCopying = false;
	bool isCutting = false;
	bool isCreatingNewItem;
	bool isNewItemFolder;
	std::string newItemName;
	bool showLoadScenePopUp = false;

	std::string newPrefabName;
	bool showSaveAsPrefabPopup = false;
	char nameBuffer[128];

	ProjectSettings settings;

	static const std::unordered_set<std::string> imageExtensions;
	static const std::unordered_set<std::string> audioExtensions;

	static const std::string FOLDER_ICON_PATH;
	static const std::string MATERIAL_ICON_PATH;
	static const std::string IMAGE_ICON_PATH;
	static const std::string SCENE_ICON_PATH;
	static const std::string MESH_ICON_PATH;
	static const std::string AUDIO_ICON_PATH;
	static const std::string DEFAULT_ICON_PATH;
	static const std::string SCRIPT_ICON_PATH;

	void DrawMainLayout();
	void DrawContentArea();
	void DrawDirectoryTree();
	void DrawBreadcrumbs();
	void DrawFolderContents(const std::filesystem::path& path);
	void DrawActionButtons();
	void DrawSettings();
	void DrawGridItem(const std::filesystem::path& entry, const std::string& filename);
	void DrawRenamingItem(const std::filesystem::path& entry, const std::string& filename, Image* icon);
	void DrawNormalItem(const std::filesystem::path& entry, const std::string& filename, Image* icon, bool isDirectory);
	void DrawNewItemCreation();
	void DrawTruncatedLabel(const std::string& filename);
	void DrawDragPreview(const std::filesystem::path& entry, Image* icon);

	void HandleItemInteractions(const std::filesystem::path& entry, const std::string& filename, Image* icon, bool isDirectory);
	void HandleFileSelection(const std::filesystem::path& filePath);
	void HandleRename(const std::filesystem::path& entry, const char* newName);
	void HandleShortcuts();

	void StartDirectoryListing(const std::filesystem::path& path);
	void SortDirectoryEntries();
	void ShowContextMenu();
	void CreateNewItem(const std::filesystem::path& path, bool isFolder);
	void CopyItem(const std::filesystem::path& path, bool isCut);
	void PasteItem(const std::filesystem::path& path);
	void DuplicateItem(const std::filesystem::path& path);

	Image* GetIconForFile(const std::filesystem::path& filePath);
	Image* GetImageThumbnail(const std::filesystem::path& imagePath);
	void ProcessPendingThumbnails();
	void CleanupThumbnailCache();

	bool IsImageFile(const std::filesystem::path& path) const;
	bool IsAudioFile(const std::filesystem::path& path) const;
	bool IsValidFileName(const std::string& name) const;
	void ShowErrorDialog(const std::string& title, const std::string& message);
	std::string GetFileExtensionLower(const std::filesystem::path& path) const;
};