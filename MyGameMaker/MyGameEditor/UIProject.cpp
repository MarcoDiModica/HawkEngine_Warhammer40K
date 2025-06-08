#include <filesystem>
#include <string>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <future>
#include <mutex>
#include <functional>
#include <stack>
#include <cctype>
#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>

#include <imgui.h>

#include "UIProject.h"
#include "App.h"
#include "MyGUI.h"
#include "MyGameEngine/types.h"
#include <MyGameEngine/PrefabManager.h>
#include "DragDropManager.h"

const std::string UIProject::FOLDER_ICON_PATH = "EngineAssets/folder.png";
const std::string UIProject::MATERIAL_ICON_PATH = "EngineAssets/material.png";
const std::string UIProject::IMAGE_ICON_PATH = "EngineAssets/image.png";
const std::string UIProject::SCENE_ICON_PATH = "EngineAssets/scene.png";
const std::string UIProject::MESH_ICON_PATH = "EngineAssets/mesh.png";
const std::string UIProject::AUDIO_ICON_PATH = "EngineAssets/audio.png";
const std::string UIProject::DEFAULT_ICON_PATH = "EngineAssets/default.png";
const std::string UIProject::SCRIPT_ICON_PATH = "EngineAssets/cscript.png";

const std::unordered_set<std::string> UIProject::imageExtensions = {
	".png", ".jpg", ".jpeg", ".bmp", ".tga", ".gif", ".tiff", ".tif"
};

const std::unordered_set<std::string> UIProject::audioExtensions = {
	".wav", ".ogg", ".mp3", ".flac", ".aac", ".wma", ".m4a"
};

UIProject::UIProject(UIType type, std::string name) : UIElement(type, name)
{
	directoryPath = LIBRARY_PATH;
	assetsPath = ASSETS_PATH;
	selectedDirectory = directoryPath;
	currentSceneFile = "";

	iconCache[".folder"] = new Image();
	iconCache[".folder"]->LoadTexture(FOLDER_ICON_PATH);
	iconCache[".mat"] = new Image();
	iconCache[".mat"]->LoadTexture(MATERIAL_ICON_PATH);
	iconCache[".image"] = new Image();
	iconCache[".image"]->LoadTexture(IMAGE_ICON_PATH);
	iconCache[".scene"] = new Image();
	iconCache[".scene"]->LoadTexture(SCENE_ICON_PATH);
	iconCache[".mesh"] = new Image();
	iconCache[".mesh"]->LoadTexture(MESH_ICON_PATH);
	iconCache[".fbx"] = new Image();
	iconCache[".fbx"]->LoadTexture(MESH_ICON_PATH);
	iconCache[".audio"] = new Image();
	iconCache[".audio"]->LoadTexture(AUDIO_ICON_PATH);
	iconCache[".cs"] = new Image();
	iconCache[".cs"]->LoadTexture(SCRIPT_ICON_PATH);
	iconCache[".default"] = new Image();
	iconCache[".default"]->LoadTexture(DEFAULT_ICON_PATH);

	currentSortOption = SortOption::Name;
	sortAscending = true;
	isCreatingNewItem = false;
	isNewItemFolder = false;
	newItemName = "New Item";
	showSaveAsPrefabPopup = false;
	newPrefabName = "";
	memset(nameBuffer, 0, sizeof(nameBuffer));

	StartDirectoryListing(directoryPath);
}

UIProject::~UIProject()
{
	if (directoryListingFuture.valid()) {
		try {
			directoryListingFuture.wait();
		}
		catch (...) {}
	}

	{
		std::lock_guard<std::mutex> lock(iconCacheMutex);
		for (auto& pair : iconCache) {
			delete pair.second;
		}
		iconCache.clear();
	}

	{
		std::lock_guard<std::mutex> lock(thumbnailMutex);
		for (auto& pair : imagePreviewCache) {
			delete pair.second.image;
		}
		imagePreviewCache.clear();
	}
}

bool UIProject::Draw()
{
	ImGuiWindowFlags projectFlags = ImGuiWindowFlags_NoCollapse;
	ImGuiWindowClass windowClass;

	if (firstDraw) {
		ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
		firstDraw = false;
	}

	ImGui::SetNextWindowClass(&windowClass);
	windowClass.DockingAllowUnclassed = false;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
	bool windowActive = ImGui::Begin("Library", &enabled, projectFlags);
	ImGui::PopStyleVar();

	if (!windowActive) {
		ImGui::End();
		return true;
	}

	ProcessPendingThumbnails();
	CleanupThumbnailCache();
	HandleShortcuts();
	DrawMainLayout();

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) &&
		ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
		!ImGui::IsAnyItemHovered() &&
		!ImGui::IsPopupOpen("ContextMenu") &&
		!ImGui::IsPopupOpen("CreateMenu") &&
		!ImGui::IsPopupOpen("SortOptions") &&
		!isCreatingNewItem &&
		renamePath.empty()) {
		selectedFile.clear();
	}

	if (!ImGui::IsWindowFocused()) {
		selectedFile.clear();
	}

	if (showLoadScenePopUp) {
		ImGui::OpenPopup("Load Scene");
	}

	if (ImGui::BeginPopupModal("Load Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Do you want to load this scene?");
		ImGui::Separator();

		if (ImGui::Button("Load", ImVec2(150, 0))) {
			Application->scene_serializer->DeSerialize(currentSceneFile);
			showLoadScenePopUp = false;
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Save and Load", ImVec2(150, 0))) {
			Application->scene_serializer->Serialize(Application->root->currentScene->GetName());
			Application->scene_serializer->DeSerialize(currentSceneFile);
			showLoadScenePopUp = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			showLoadScenePopUp = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
	return true;
}

void UIProject::DrawMainLayout()
{
	static ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;

	if (ImGui::BeginTable("MainTable", 2, tableFlags)) {
		ImGui::TableSetupColumn("DirectoryTree", ImGuiTableColumnFlags_WidthFixed, 200.0f);
		ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImVec2 availSpaceTree = ImGui::GetContentRegionAvail();
		if (ImGui::BeginChild("DirectoryTreeScroll", availSpaceTree, true, ImGuiWindowFlags_HorizontalScrollbar)) {
			DrawDirectoryTree();
		}
		ImGui::EndChild();

		ImGui::TableSetColumnIndex(1);
		ImVec2 availSpaceContent = ImGui::GetContentRegionAvail();
		if (ImGui::BeginChild("ContentAreaScroll", availSpaceContent, true, ImGuiWindowFlags_HorizontalScrollbar)) {
			DrawContentArea();
		}
		ImGui::EndChild();

		ImGui::EndTable();
	}

	if (showDeletePopup) {
		ImGui::OpenPopup("ConfirmDelete");
	}

	if (ImGui::BeginPopupModal("ConfirmDelete", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Are you sure you want to delete this %s?",
			std::filesystem::is_directory(selectedFile) ? "folder" : "file");

		if (ImGui::Button("OK", ImVec2(120, 0))) {
			try {
				std::filesystem::remove_all(selectedFile);
				selectedFile.clear();
				StartDirectoryListing(selectedDirectory);
			}
			catch (const std::filesystem::filesystem_error& ex) {
				ShowErrorDialog("Delete Error", ex.what());
			}
			showDeletePopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			showDeletePopup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void UIProject::DrawContentArea()
{
	DrawBreadcrumbs();
	DrawActionButtons();
	DrawSettings();

	ImGui::Separator();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	DrawFolderContents(selectedDirectory);

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
			GameObject* go = *(GameObject**)payload->Data;
			if (go) {
				newPrefabName = go->GetName();
				strncpy(nameBuffer, newPrefabName.c_str(), sizeof(nameBuffer) - 1);
				nameBuffer[sizeof(nameBuffer) - 1] = '\0';
				showSaveAsPrefabPopup = true;
				ImGui::OpenPopup("##SaveAsPrefabPopup");
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		if (!ImGui::IsAnyItemHovered()) {
			selectedFile.clear();
		}
		ImGui::OpenPopup("ContextMenu");
	}

	if (ImGui::BeginPopup("ContextMenu")) {
		ShowContextMenu();
		ImGui::EndPopup();
	}

	if (showSaveAsPrefabPopup &&
		ImGui::BeginPopupModal("##SaveAsPrefabPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Enter a name to save the prefab:");
		ImGui::InputText("Prefab Name", nameBuffer, sizeof(nameBuffer));

		if (ImGui::Button("Save", ImVec2(120, 0))) {
			std::string finalName(nameBuffer);
			if (finalName.empty() || !IsValidFileName(finalName)) {
				ShowErrorDialog("Invalid Name", "Please enter a valid prefab name.");
			}
			else {
				std::string path = PrefabManager::GetUniquePrefabPath(finalName);
				PrefabManager::EnsurePrefabDirectoryExists();
				if (DragDropManager::draggedObject) {
					PrefabManager::SavePrefab(DragDropManager::draggedObject->shared_from_this(), path);
					LOG(LogType::LOG_INFO, "Prefab saved: %s", path.c_str());
				}
				DragDropManager::draggedObject = nullptr;
				showSaveAsPrefabPopup = false;
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			DragDropManager::draggedObject = nullptr;
			showSaveAsPrefabPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
}

void UIProject::DrawSettings()
{
	ImGui::Separator();

	ImGui::Text("Icon Size:");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	if (ImGui::SliderFloat("##IconSize", &settings.iconSize,
		settings.minIconSize, settings.maxIconSize, "%.0f")) {
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::Checkbox("Tn", &settings.showThumbnails);

	ImGui::Separator();
}

void UIProject::DrawDirectoryTree()
{
	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth;

	const int MAX_DEPTH = 20;

	auto drawDirectoryIterative = [&](const std::filesystem::path& rootPath) {
		std::stack<std::pair<std::filesystem::path, int>> dirStack;
		std::stack<bool> nodeOpenStack;

		ImGuiTreeNodeFlags rootFlags = baseFlags | ImGuiTreeNodeFlags_DefaultOpen;
		if (rootPath == selectedDirectory) {
			rootFlags |= ImGuiTreeNodeFlags_Selected;
		}

		bool rootOpen = ImGui::TreeNodeEx(rootPath.filename().string().c_str(), rootFlags);

		if (ImGui::IsItemClicked()) {
			selectedDirectory = rootPath;
			selectedFile.clear();
			StartDirectoryListing(selectedDirectory);
		}

		if (rootOpen) {
			try {
				for (const auto& entry : std::filesystem::directory_iterator(rootPath)) {
					if (std::filesystem::is_directory(entry)) {
						dirStack.push({ entry.path(), 1 });
					}
				}
			}
			catch (const std::filesystem::filesystem_error& ex) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", ex.what());
			}

			while (!dirStack.empty()) {
				auto [currentPath, depth] = dirStack.top();
				dirStack.pop();

				if (depth >= MAX_DEPTH) continue;

				// Indent based on depth
				for (int i = 0; i < depth; ++i) {
					ImGui::Indent();
				}

				std::string name = currentPath.filename().string();
				ImGuiTreeNodeFlags nodeFlags = baseFlags;

				if (currentPath == selectedDirectory) {
					nodeFlags |= ImGuiTreeNodeFlags_Selected;
				}

				bool nodeOpen = ImGui::TreeNodeEx(name.c_str(), nodeFlags);

				if (ImGui::IsItemClicked()) {
					selectedDirectory = currentPath;
					selectedFile.clear();
					StartDirectoryListing(selectedDirectory);
				}

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
						draggedItemPath = std::filesystem::path((const char*)payload->Data);
						if (draggedItemPath != currentPath && draggedItemPath.parent_path() != currentPath) {
							try {
								std::filesystem::rename(draggedItemPath, currentPath / draggedItemPath.filename());
								StartDirectoryListing(selectedDirectory);
							}
							catch (const std::filesystem::filesystem_error& ex) {
								ShowErrorDialog("Move Error", ex.what());
							}
						}
					}
					ImGui::EndDragDropTarget();
				}

				if (nodeOpen) {
					try {
						for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
							if (std::filesystem::is_directory(entry)) {
								dirStack.push({ entry.path(), depth + 1 });
							}
						}
					}
					catch (const std::filesystem::filesystem_error& ex) {
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", ex.what());
					}
					ImGui::TreePop();
				}

				for (int i = 0; i < depth; ++i) {
					ImGui::Unindent();
				}
			}

			ImGui::TreePop();
		}
		};

	drawDirectoryIterative(directoryPath);
	drawDirectoryIterative(assetsPath);
}

void UIProject::DrawBreadcrumbs()
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 0.5f));

	std::vector<std::filesystem::path> pathParts;
	std::filesystem::path tempPath = selectedDirectory;

	while (tempPath != tempPath.root_path() && tempPath != directoryPath.parent_path()) {
		pathParts.push_back(tempPath);
		tempPath = tempPath.parent_path();
	}
	std::reverse(pathParts.begin(), pathParts.end());

	if (ImGui::Button(directoryPath.filename().string().c_str())) {
		selectedDirectory = directoryPath;
		selectedFile.clear();
		StartDirectoryListing(selectedDirectory);
	}

	for (const auto& part : pathParts) {
		ImGui::SameLine();
		ImGui::Text(">");
		ImGui::SameLine();
		if (ImGui::Button(part.filename().string().c_str())) {
			selectedDirectory = part;
			selectedFile.clear();
			StartDirectoryListing(selectedDirectory);
		}
	}

	ImGui::PopStyleColor(3);
}

void UIProject::DrawFolderContents(const std::filesystem::path& path)
{
	static char filter[256] = "";
	ImGui::InputText("Filter", filter, IM_ARRAYSIZE(filter));

	if (isLoading.load()) {
		if (directoryListingFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			std::lock_guard<std::recursive_mutex> lock(entriesMutex);
			currentDirectoryEntries = directoryListingFuture.get();
			isLoading.store(false);
			SortDirectoryEntries();
		}
		else {
			ImGui::Text("Loading...");
			return;
		}
	}

	if (ImGui::Button("Sort")) {
		ImGui::OpenPopup("SortOptions");
	}

	if (ImGui::BeginPopup("SortOptions")) {
		if (ImGui::Selectable("Name", currentSortOption == SortOption::Name)) {
			currentSortOption = SortOption::Name;
			SortDirectoryEntries();
		}
		if (ImGui::Selectable("Type", currentSortOption == SortOption::Type)) {
			currentSortOption = SortOption::Type;
			SortDirectoryEntries();
		}
		if (ImGui::Selectable("Size", currentSortOption == SortOption::Size)) {
			currentSortOption = SortOption::Size;
			SortDirectoryEntries();
		}
		if (ImGui::Selectable("Last Modified", currentSortOption == SortOption::LastModified)) {
			currentSortOption = SortOption::LastModified;
			SortDirectoryEntries();
		}

		ImGui::Separator();
		if (ImGui::Checkbox("Ascending", &sortAscending)) {
			SortDirectoryEntries();
		}

		ImGui::EndPopup();
	}

	float contentRegionWidth = ImGui::GetContentRegionAvail().x;
	const float totalSize = settings.iconSize + 24.0f;
	int itemsPerRow = static_cast<int>(std::max(1.0f, (contentRegionWidth - 5.0f) / totalSize));
	if (itemsPerRow < 1) itemsPerRow = 1;

	if (ImGui::BeginTable("FileGrid", itemsPerRow, ImGuiTableFlags_NoBordersInBody)) {
		if (isCreatingNewItem) {
			DrawNewItemCreation();
		}

		std::lock_guard<std::recursive_mutex> lock(entriesMutex);
		for (const auto& entry : currentDirectoryEntries) {
			std::string filename = entry.filename().string();

			if (filter[0] != '\0') {
				std::string filterLower(filter);
				std::transform(filterLower.begin(), filterLower.end(), filterLower.begin(), ::tolower);
				std::string filenameLower = filename;
				std::transform(filenameLower.begin(), filenameLower.end(), filenameLower.begin(), ::tolower);

				if (filenameLower.find(filterLower) == std::string::npos) {
					continue;
				}
			}

			DrawGridItem(entry, filename);

			if (std::filesystem::is_directory(entry)) {
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
						draggedItemPath = std::filesystem::path((const char*)payload->Data);
						if (draggedItemPath != entry && draggedItemPath.parent_path() != entry) {
							try {
								std::filesystem::rename(draggedItemPath, entry / draggedItemPath.filename());
								StartDirectoryListing(selectedDirectory);
							}
							catch (const std::filesystem::filesystem_error& ex) {
								ShowErrorDialog("Move Error", ex.what());
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
			}
		}
		ImGui::EndTable();
	}
}

void UIProject::DrawGridItem(const std::filesystem::path& entry, const std::string& filename)
{
	ImGui::TableNextColumn();
	ImGui::PushID(filename.c_str());
	ImGui::BeginGroup();

	Image* icon = GetIconForFile(entry);
	bool isDirectory = std::filesystem::is_directory(entry);
	bool isBeingRenamed = (entry == renamePath);

	if (isBeingRenamed) {
		DrawRenamingItem(entry, filename, icon);
	}
	else {
		DrawNormalItem(entry, filename, icon, isDirectory);
	}

	ImGui::EndGroup();
	ImGui::PopID();
}

void UIProject::DrawRenamingItem(const std::filesystem::path& entry, const std::string& filename, Image* icon)
{
	static char renameBuf[256];
	static bool initRename = true;

	ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(icon->id())),
		ImVec2(settings.iconSize, settings.iconSize));

	if (initRename) {
		strncpy(renameBuf, filename.c_str(), sizeof(renameBuf) - 1);
		renameBuf[sizeof(renameBuf) - 1] = '\0';
		initRename = false;
		ImGui::SetKeyboardFocusHere();
	}

	ImGui::PushItemWidth(settings.iconSize + 4);
	if (ImGui::InputText("##rename", renameBuf, IM_ARRAYSIZE(renameBuf),
		ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
		if (IsValidFileName(renameBuf)) {
			HandleRename(entry, renameBuf);
		}
		else {
			ShowErrorDialog("Invalid Name", "The filename contains invalid characters.");
		}
		initRename = true;
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
		renamePath.clear();
		initRename = true;
	}

	if (!ImGui::IsItemFocused() && ImGui::IsMouseClicked(0)) {
		if (!ImGui::IsItemHovered()) {
			renamePath.clear();
			initRename = true;
		}
	}
	ImGui::PopItemWidth();
}

void UIProject::DrawNormalItem(const std::filesystem::path& entry, const std::string& filename,
	Image* icon, bool isDirectory)
{
	bool isSelected = (entry == selectedFile);

	if (isSelected) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 0.5f));
	}

	bool clicked = ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(icon->id())),
		ImVec2(settings.iconSize, settings.iconSize));

	if (isSelected) {
		ImGui::PopStyleColor();
	}

	if (clicked) {
		selectedFile = entry;
	}

	HandleItemInteractions(entry, filename, icon, isDirectory);
	DrawTruncatedLabel(filename);
}

void UIProject::HandleItemInteractions(const std::filesystem::path& entry, const std::string& filename,
	Image* icon, bool isDirectory)
{
	if (ImGui::IsItemHovered()) {
		if (ImGui::IsMouseDoubleClicked(0)) {
			if (isDirectory) {
				selectedDirectory = entry;
				selectedFile.clear();
				StartDirectoryListing(selectedDirectory);
			}
			else {
				HandleFileSelection(entry);
			}
		}

		ImGui::BeginTooltip();
		ImGui::Text("%s", filename.c_str());
		if (!isDirectory && !std::filesystem::is_empty(entry)) {
			auto size = std::filesystem::file_size(entry);
			ImGui::Text("Size: %.2f MB", size / 1048576.0);
		}
		ImGui::EndTooltip();
	}

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		selectedFile = entry;
		ImGui::OpenPopup("ContextMenu");
	}

	if (ImGui::BeginDragDropSource()) {
		std::string fullPath = entry.string();
		ImGui::SetDragDropPayload("ASSET_PATH", fullPath.c_str(), fullPath.length() + 1);

		if (entry.extension() == ".yaml" && entry.string().find(".prefab") != std::string::npos) {
			ImGui::SetDragDropPayload("ASSET_PATH", fullPath.c_str(), fullPath.length() + 1);
		}

		DrawDragPreview(entry, icon);
		draggedItemPath = entry;
		ImGui::EndDragDropSource();
	}
}

void UIProject::DrawDragPreview(const std::filesystem::path& entry, Image* icon)
{
	ImGui::BeginTooltip();

	if (isCutting && draggedItemPath == entry) {
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
	}

	ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(icon->id())),
		ImVec2(settings.iconSize * 0.75f, settings.iconSize * 0.75f));
	ImGui::SameLine();
	ImGui::Text("%s", entry.filename().string().c_str());

	if (isCutting && draggedItemPath == entry) {
		ImGui::PopStyleVar();
	}

	ImGui::EndTooltip();
}

void UIProject::DrawNewItemCreation()
{
	ImGui::TableNextColumn();
	ImGui::PushID("##NewItemCreation");
	ImGui::BeginGroup();

	Image* icon = isNewItemFolder ? iconCache[".folder"] : iconCache[".default"];
	ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(icon->id())),
		ImVec2(settings.iconSize, settings.iconSize));

	ImGui::SetKeyboardFocusHere();
	char nameBuf[256];
	strncpy(nameBuf, newItemName.c_str(), sizeof(nameBuf) - 1);
	nameBuf[sizeof(nameBuf) - 1] = '\0';

	ImGui::PushItemWidth(settings.iconSize + 4);
	if (ImGui::InputText("##newItemName", nameBuf, IM_ARRAYSIZE(nameBuf),
		ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
		newItemName = nameBuf;

		if (!IsValidFileName(newItemName)) {
			ShowErrorDialog("Invalid Name", "The filename contains invalid characters.");
		}
		else {
			try {
				if (isNewItemFolder) {
					std::filesystem::create_directory(newItemPath / newItemName);
				}
				else {
					std::ofstream newFile(newItemPath / newItemName);
					newFile.close();
				}
				isCreatingNewItem = false;
				StartDirectoryListing(selectedDirectory);
			}
			catch (const std::filesystem::filesystem_error& ex) {
				ShowErrorDialog("Create Error", ex.what());
			}
		}
	}

	newItemName = nameBuf;

	if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
		isCreatingNewItem = false;
	}

	if (!ImGui::IsItemFocused() && ImGui::IsMouseClicked(0)) {
		if (!ImGui::IsItemHovered()) {
			isCreatingNewItem = false;
		}
	}

	ImGui::PopItemWidth();
	ImGui::EndGroup();
	ImGui::PopID();
}

void UIProject::DrawActionButtons()
{
	if (selectedDirectory != directoryPath && selectedDirectory != assetsPath) {
		if (ImGui::Button("Back")) {
			selectedDirectory = selectedDirectory.parent_path();
			selectedFile.clear();
			StartDirectoryListing(selectedDirectory);
		}
		ImGui::SameLine();
	}

	if (ImGui::Button("Refresh")) {
		StartDirectoryListing(selectedDirectory);
	}
	ImGui::SameLine();

	if (ImGui::Button("Open In Explorer")) {
		std::string path = selectedDirectory.string();
		if (!path.empty()) {
			ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Create")) {
		ImGui::OpenPopup("CreateMenu");
	}

	if (ImGui::BeginPopup("CreateMenu")) {
		if (ImGui::MenuItem("Folder")) {
			CreateNewItem(selectedDirectory, true);
		}
		if (ImGui::MenuItem("File")) {
			CreateNewItem(selectedDirectory, false);
		}
		ImGui::EndPopup();
	}
}

void UIProject::StartDirectoryListing(const std::filesystem::path& path)
{
	isLoading.store(true);

	{
		std::lock_guard<std::recursive_mutex> lock(entriesMutex);
		currentDirectoryEntries.clear();
	}

	directoryListingFuture = std::async(std::launch::async, [path]() {
		std::vector<std::filesystem::path> entries;
		try {
			for (const auto& entry : std::filesystem::directory_iterator(path)) {
				entries.push_back(entry.path());
			}
		}
		catch (const std::exception& e) {
			LOG(LogType::LOG_ERROR, "Error listing directory: %s", e.what());
		}
		return entries;
		});
}

void UIProject::SortDirectoryEntries()
{
	std::lock_guard<std::recursive_mutex> lock(entriesMutex);

	std::sort(currentDirectoryEntries.begin(), currentDirectoryEntries.end(),
		[&](const std::filesystem::path& a, const std::filesystem::path& b) {
			bool aIsDir = std::filesystem::is_directory(a);
			bool bIsDir = std::filesystem::is_directory(b);

			if (aIsDir != bIsDir)
				return aIsDir > bIsDir;

			bool result = false;
			switch (currentSortOption) {
			case SortOption::Name:
				result = a.filename().string() < b.filename().string();
				break;
			case SortOption::Type:
				result = GetFileExtensionLower(a) < GetFileExtensionLower(b);
				break;
			case SortOption::Size:
				if (!aIsDir && !bIsDir) {
					try {
						result = std::filesystem::file_size(a) < std::filesystem::file_size(b);
					}
					catch (...) {
						result = false;
					}
				}
				break;
			case SortOption::LastModified:
				try {
					result = std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b);
				}
				catch (...) {
					result = false;
				}
				break;
			}
			return sortAscending ? result : !result;
		});
}

void UIProject::HandleFileSelection(const std::filesystem::path& filePath)
{
	if (filePath.extension() == ".scene") {
		currentSceneFile = filePath.string();
		showLoadScenePopUp = true;
	}
	else if (filePath.extension() == ".cs") {
		std::string scriptPath = filePath.string();
		if (!std::filesystem::exists(scriptPath)) {
			ShowErrorDialog("File Not Found", "Script file does not exist.");
		}
		else {
			HINSTANCE result = ShellExecuteA(nullptr, "open", scriptPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
			if ((int)result <= 32) {
				ShowErrorDialog("Open Error", "Failed to open script file.");
			}
			else {
				LOG(LogType::LOG_INFO, "Successfully opened script: %s", scriptPath.c_str());
			}
		}
	}
	else if (filePath.extension() == ".yaml" && filePath.string().find(".prefab") != std::string::npos) {
		auto prefab = PrefabManager::LoadPrefab(filePath.string());
		if (!prefab) {
			ShowErrorDialog("Load Error", "Failed to load prefab.");
		}
	}
	else if (filePath.extension() == ".fbx") {
		Application->root->CreateGameObjectWithPath(filePath.string());
	}
}

void UIProject::HandleRename(const std::filesystem::path& entry, const char* newName)
{
	try {
		std::filesystem::path newPath = entry.parent_path() / newName;
		std::filesystem::rename(entry, newPath);
		if (selectedFile == entry) {
			selectedFile = newPath;
		}
		renamePath.clear();
		StartDirectoryListing(selectedDirectory);
	}
	catch (const std::filesystem::filesystem_error& ex) {
		ShowErrorDialog("Rename Error", ex.what());
	}
}

Image* UIProject::GetIconForFile(const std::filesystem::path& filePath)
{
	std::lock_guard<std::mutex> lock(iconCacheMutex);

	if (std::filesystem::is_directory(filePath)) {
		return iconCache[".folder"];
	}

	std::string extension = GetFileExtensionLower(filePath);

	if (settings.showThumbnails && IsImageFile(filePath)) {
		return GetImageThumbnail(filePath);
	}
	else if (iconCache.count(extension)) {
		return iconCache[extension];
	}
	else if (IsAudioFile(filePath)) {
		return iconCache[".audio"];
	}
	else {
		return iconCache[".default"];
	}
}

void UIProject::ShowContextMenu()
{
	if (selectedFile.empty()) {
		if (ImGui::MenuItem("New Folder")) {
			CreateNewItem(selectedDirectory, true);
		}
		if (ImGui::MenuItem("New File")) {
			CreateNewItem(selectedDirectory, false);
		}
		if (ImGui::MenuItem("Refresh")) {
			StartDirectoryListing(selectedDirectory);
		}

		if (!copiedItemPath.empty() && ImGui::MenuItem(isCutting ? "Paste (Move)" : "Paste")) {
			PasteItem(selectedDirectory);
		}
	}
	else {
		if (std::filesystem::is_directory(selectedFile)) {
			if (ImGui::MenuItem("Open")) {
				selectedDirectory = selectedFile;
				selectedFile.clear();
				StartDirectoryListing(selectedDirectory);
			}
			if (ImGui::MenuItem("Rename")) {
				renamePath = selectedFile;
			}
			if (ImGui::MenuItem("Delete")) {
				showDeletePopup = true;
			}
			if (ImGui::MenuItem("Copy")) {
				CopyItem(selectedFile, false);
			}
			if (ImGui::MenuItem("Cut")) {
				CopyItem(selectedFile, true);
			}
			if (ImGui::MenuItem("Duplicate")) {
				DuplicateItem(selectedFile);
			}
			if (!copiedItemPath.empty() && ImGui::MenuItem(isCutting ? "Paste (Move)" : "Paste")) {
				PasteItem(selectedFile);
			}
		}
		else {
			if (selectedFile.extension() == ".scene" && ImGui::MenuItem("Load Scene")) {
				currentSceneFile = selectedFile.string();
				showLoadScenePopUp = true;
			}
			if (ImGui::MenuItem("Rename")) {
				renamePath = selectedFile;
			}
			if (ImGui::MenuItem("Delete")) {
				showDeletePopup = true;
			}
			if (ImGui::MenuItem("Copy")) {
				CopyItem(selectedFile, false);
			}
			if (ImGui::MenuItem("Cut")) {
				CopyItem(selectedFile, true);
			}
			if (ImGui::MenuItem("Duplicate")) {
				DuplicateItem(selectedFile);
			}

			if (selectedFile.extension() == ".cs" && ImGui::MenuItem("Open in Editor")) {
				HandleFileSelection(selectedFile);
			}
		}
	}
}

void UIProject::CopyItem(const std::filesystem::path& itemPath, bool isCut)
{
	copiedItemPath = itemPath;
	isCopying = !isCut;
	isCutting = isCut;
}

void UIProject::DuplicateItem(const std::filesystem::path& itemPath)
{
	try {
		std::filesystem::path destinationPath;
		std::string stem = itemPath.stem().string();
		std::string ext = itemPath.extension().string();
		int counter = 1;

		destinationPath = itemPath.parent_path() / (stem + " - Copy" + ext);

		while (std::filesystem::exists(destinationPath)) {
			destinationPath = itemPath.parent_path() /
				(stem + " - Copy (" + std::to_string(counter) + ")" + ext);
			counter++;
		}

		if (std::filesystem::is_directory(itemPath)) {
			std::filesystem::copy(itemPath, destinationPath,
				std::filesystem::copy_options::recursive);
		}
		else {
			std::filesystem::copy(itemPath, destinationPath);
		}

		StartDirectoryListing(selectedDirectory);
	}
	catch (const std::filesystem::filesystem_error& ex) {
		ShowErrorDialog("Duplicate Error", ex.what());
	}
}

void UIProject::PasteItem(const std::filesystem::path& destinationDir)
{
	if (copiedItemPath.empty()) return;

	try {
		std::filesystem::path destinationPath;

		if (std::filesystem::is_regular_file(destinationDir)) {
			destinationPath = destinationDir.parent_path() / copiedItemPath.filename();
		}
		else {
			destinationPath = destinationDir / copiedItemPath.filename();
		}

		if (std::filesystem::exists(destinationPath)) {
			std::string stem = destinationPath.stem().string();
			std::string ext = destinationPath.extension().string();
			int counter = 1;

			while (std::filesystem::exists(destinationPath)) {
				destinationPath = destinationPath.parent_path() /
					(stem + " (" + std::to_string(counter) + ")" + ext);
				counter++;
			}
		}

		if (isCutting) {
			std::filesystem::rename(copiedItemPath, destinationPath);
		}
		else {
			if (std::filesystem::is_directory(copiedItemPath)) {
				std::filesystem::copy(copiedItemPath, destinationPath,
					std::filesystem::copy_options::recursive);
			}
			else {
				std::filesystem::copy(copiedItemPath, destinationPath);
			}
		}

		StartDirectoryListing(selectedDirectory);

		copiedItemPath.clear();
		isCopying = false;
		isCutting = false;
	}
	catch (const std::filesystem::filesystem_error& ex) {
		ShowErrorDialog("Paste Error", ex.what());
	}
}

void UIProject::CreateNewItem(const std::filesystem::path& path, bool isFolder)
{
	isCreatingNewItem = true;
	isNewItemFolder = isFolder;
	newItemPath = path;
	newItemName = isFolder ? "New Folder" : "New File.txt";
}

void UIProject::DrawTruncatedLabel(const std::string& filename)
{
	std::string shortName = filename;
	float textWidth = ImGui::CalcTextSize(shortName.c_str()).x;

	if (textWidth > settings.iconSize) {
		while (ImGui::CalcTextSize((shortName + "...").c_str()).x > settings.iconSize && shortName.length() > 3) {
			shortName.pop_back();
		}
		shortName += "...";
	}

	float cursorX = ImGui::GetCursorPosX();
	ImGui::SetCursorPosX(cursorX + (settings.iconSize - ImGui::CalcTextSize(shortName.c_str()).x) * 0.5f);
	ImGui::Text("%s", shortName.c_str());
}

void UIProject::HandleShortcuts()
{
	if (!ImGui::IsWindowFocused()) return;

	if (ImGui::IsKeyPressed(ImGuiKey_Delete) && !selectedFile.empty()) {
		showDeletePopup = true;
	}

	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
		if (ImGui::IsKeyPressed(ImGuiKey_C) && !selectedFile.empty()) {
			CopyItem(selectedFile, false);
		}

		if (ImGui::IsKeyPressed(ImGuiKey_X) && !selectedFile.empty()) {
			CopyItem(selectedFile, true);
		}

		if (ImGui::IsKeyPressed(ImGuiKey_V) && !copiedItemPath.empty()) {
			PasteItem(selectedDirectory);
		}

		if (ImGui::IsKeyPressed(ImGuiKey_D) && !selectedFile.empty()) {
			DuplicateItem(selectedFile);
		}
	}

	if (ImGui::IsKeyPressed(ImGuiKey_F2) && !selectedFile.empty()) {
		renamePath = selectedFile;
	}
}

bool UIProject::IsImageFile(const std::filesystem::path& path) const
{
	std::string ext = GetFileExtensionLower(path);
	return imageExtensions.find(ext) != imageExtensions.end();
}

bool UIProject::IsAudioFile(const std::filesystem::path& path) const
{
	std::string ext = GetFileExtensionLower(path);
	return audioExtensions.find(ext) != audioExtensions.end();
}

std::string UIProject::GetFileExtensionLower(const std::filesystem::path& path) const
{
	std::string ext = path.extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	return ext;
}

bool UIProject::IsValidFileName(const std::string& name) const
{
	static const std::string invalidChars = "\\/:*?\"<>|";
	return name.find_first_of(invalidChars) == std::string::npos && !name.empty();
}

void UIProject::ShowErrorDialog(const std::string& title, const std::string& message)
{
	LOG(LogType::LOG_ERROR, "%s: %s", title.c_str(), message.c_str());
}

Image* UIProject::GetImageThumbnail(const std::filesystem::path& imagePath)
{
	std::lock_guard<std::mutex> lock(thumbnailMutex);

	auto it = imagePreviewCache.find(imagePath);
	if (it != imagePreviewCache.end()) {
		it->second.lastAccess = std::chrono::steady_clock::now();
		return it->second.image;
	}

	if (pendingThumbnails.find(imagePath) == pendingThumbnails.end()) {
		pendingThumbnails.insert(imagePath);
	}

	return iconCache[".image"];
}

void UIProject::ProcessPendingThumbnails()
{
	if (pendingThumbnails.empty() || processingThumbnails.exchange(true))
		return;

	std::lock_guard<std::mutex> lock(thumbnailMutex);

	int processed = 0;
	auto it = pendingThumbnails.begin();

	while (it != pendingThumbnails.end() && processed < settings.maxTexturesPerFrame) {
		auto path = *it;

		if (imagePreviewCache.find(path) == imagePreviewCache.end()) {
			Image* thumbnail = new Image();
			if (thumbnail->LoadTexture(path.string())) {
				ThumbnailEntry entry;
				entry.image = thumbnail;
				entry.lastAccess = std::chrono::steady_clock::now();
				imagePreviewCache[path] = entry;
			}
			else {
				delete thumbnail;
			}
		}

		it = pendingThumbnails.erase(it);
		processed++;
	}

	processingThumbnails.store(false);
}

void UIProject::CleanupThumbnailCache()
{
	std::lock_guard<std::mutex> lock(thumbnailMutex);

	if (imagePreviewCache.size() <= settings.thumbnailCacheSize)
		return;

	std::vector<std::pair<std::filesystem::path, std::chrono::steady_clock::time_point>> entries;
	for (const auto& pair : imagePreviewCache) {
		entries.push_back({ pair.first, pair.second.lastAccess });
	}

	std::sort(entries.begin(), entries.end(),
		[](const auto& a, const auto& b) {
			return a.second < b.second;
		});

	int toRemove = imagePreviewCache.size() - settings.thumbnailCacheSize;
	for (int i = 0; i < toRemove && i < entries.size(); ++i) {
		auto it = imagePreviewCache.find(entries[i].first);
		if (it != imagePreviewCache.end()) {
			delete it->second.image;
			imagePreviewCache.erase(it);
		}
	}
}