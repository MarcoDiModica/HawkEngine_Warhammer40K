#include "UIProject.h"
#include "App.h"
#include "imgui.h"
#include "MyGUI.h"
#include "MyGameEngine/types.h"
#include "../MyScriptingEngine/ScriptComponent.h"
#include <filesystem>
#include <string>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <future>
#include <functional>
#define NOMINMAX
#include <Windows.h>

const std::string FOLDER_ICON_PATH = "EngineAssets/folder.png";
const std::string MATERIAL_ICON_PATH = "EngineAssets/material.png";
const std::string IMAGE_ICON_PATH = "EngineAssets/image.png";
const std::string SCENE_ICON_PATH = "EngineAssets/scene.png";
const std::string MESH_ICON_PATH = "EngineAssets/mesh.png";
const std::string AUDIO_ICON_PATH = "EngineAssets/audio.png";
const std::string DEFAULT_ICON_PATH = "EngineAssets/default.png";
const std::string SCRIPT_ICON_PATH = "EngineAssets/cscript.png";

UIProject::UIProject(UIType type, std::string name) : UIElement(type, name)
{
    directoryPath = LIBRARY_PATH;
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

    for (auto& pair : iconCache) {
        delete pair.second;
    }
    iconCache.clear();
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

    DrawMainLayout();

    // Load Scene Popup
    if (ImGui::BeginPopupModal("Load Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Do you want to load this scene?");
        ImGui::Separator();

        if (ImGui::Button("Load without saving", ImVec2(150, 0))) {
            Application->scene_serializer->DeSerialize(currentSceneFile);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
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
        ImGui::TableSetupColumn("DirectoryTree", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        DrawDirectoryTree();

        ImGui::TableSetColumnIndex(1);
        DrawContentArea();

        ImGui::EndTable();
    }

    if (showDeletePopup) {
        ImGui::OpenPopup("ConfirmDelete");
    }
    if (ImGui::BeginPopupModal("ConfirmDelete")) {
        ImGui::Text("Are you sure you want to delete this %s?", std::filesystem::is_directory(selectedFile) ? "folder" : "file");
        if (ImGui::Button("OK")) {
            try {
                std::filesystem::remove_all(selectedFile);
                selectedFile.clear();
                StartDirectoryListing(selectedDirectory);
            }
            catch (const std::filesystem::filesystem_error& ex) {
                LOG(LogType::LOG_ERROR, "Error deleting %s: %s", std::filesystem::is_directory(selectedFile) ? "folder" : "file", ex.what());
            }
            showDeletePopup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
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

    ImGui::Separator();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    DrawFolderContents(selectedDirectory);

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

    ImGui::PopStyleVar();
}

void UIProject::DrawDirectoryTree()
{
    ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    std::function<void(const std::filesystem::path&)> drawDirectory =
        [&](const std::filesystem::path& path) {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (!std::filesystem::is_directory(entry)) continue;

                std::string name = entry.path().filename().string();
                ImGuiTreeNodeFlags nodeFlags = baseFlags;

                if (entry.path() == selectedDirectory) {
                    nodeFlags |= ImGuiTreeNodeFlags_Selected;
                }

                bool nodeOpen = ImGui::TreeNodeEx(name.c_str(), nodeFlags);

                if (ImGui::IsItemClicked()) {
                    selectedDirectory = entry.path();
                    selectedFile.clear();
                    StartDirectoryListing(selectedDirectory);
                }

                if (nodeOpen) {
                    drawDirectory(entry.path());
                    ImGui::TreePop();
                }
            }
        }
        catch (const std::filesystem::filesystem_error& ex) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", ex.what());
        }
        };

    ImGuiTreeNodeFlags rootFlags = baseFlags | ImGuiTreeNodeFlags_DefaultOpen;
    if (ImGui::TreeNodeEx(directoryPath.string().c_str(), rootFlags)) {
        drawDirectory(directoryPath);
        ImGui::TreePop();
    }
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

    if (isLoading) {
        if (directoryListingFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            currentDirectoryEntries = directoryListingFuture.get();
            isLoading = false;
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
    const float totalSize = ICON_SIZE + 16.0f;
    int itemsPerRow = static_cast<int>(std::max(1.0f, (contentRegionWidth - 5.0f) / totalSize));
    if (itemsPerRow < 1) itemsPerRow = 1;

    if (ImGui::BeginTable("FileGrid", itemsPerRow, ImGuiTableFlags_NoBordersInBody)) {
        if (isCreatingNewItem) {
            DrawNewItemCreation();
        }

        for (const auto& entry : currentDirectoryEntries) {
            std::string filename = entry.filename().string();
            if (filter[0] != '\0' && filename.find(filter) == std::string::npos) {
                continue;
            }
            DrawGridItem(entry, filename);
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

    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(icon->id())), ImVec2(ICON_SIZE, ICON_SIZE));

    if (initRename) {
        strncpy(renameBuf, filename.c_str(), sizeof(renameBuf) - 1);
        renameBuf[sizeof(renameBuf) - 1] = '\0';
        initRename = false;
        ImGui::SetKeyboardFocusHere();
    }

    ImGui::PushItemWidth(ICON_SIZE + 4);
    if (ImGui::InputText("##rename", renameBuf, IM_ARRAYSIZE(renameBuf),
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
        HandleRename(entry, renameBuf);
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

void UIProject::DrawNormalItem(const std::filesystem::path& entry, const std::string& filename, Image* icon, bool isDirectory)
{
    bool isSelected = (entry == selectedFile);

    if (isSelected) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 0.5f));
    }

    bool clicked = ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(icon->id())),
        ImVec2(ICON_SIZE, ICON_SIZE));

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
        ImGui::EndTooltip();
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        selectedFile = entry;
        ImGui::OpenPopup("ContextMenu");
    }

    if (ImGui::BeginDragDropSource()) {
        std::string fullPath = entry.string();
        ImGui::SetDragDropPayload("ASSET_PATH", fullPath.c_str(), fullPath.length() + 1);
        ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(icon->id())),
            ImVec2(ICON_SIZE / 2, ICON_SIZE / 2));
        ImGui::Text("%s", filename.c_str());
        ImGui::EndDragDropSource();
    }
}

void UIProject::DrawNewItemCreation()
{
    ImGui::TableNextColumn();
    ImGui::PushID("##NewItemCreation");
    ImGui::BeginGroup();

    Image* icon = isNewItemFolder ? iconCache[".folder"] : iconCache[".default"];
    ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(icon->id())),
        ImVec2(ICON_SIZE, ICON_SIZE));

    ImGui::SetKeyboardFocusHere();
    char nameBuf[256];
    strncpy(nameBuf, newItemName.c_str(), sizeof(nameBuf) - 1);
    nameBuf[sizeof(nameBuf) - 1] = '\0';

    ImGui::PushItemWidth(ICON_SIZE + 4);
    if (ImGui::InputText("##newItemName", nameBuf, IM_ARRAYSIZE(nameBuf),
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
        try {
            newItemName = nameBuf;
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
            LOG(LogType::LOG_ERROR, "Error creating new item: %s", ex.what());
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
    if (selectedDirectory != directoryPath) {
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
    isLoading = true;
    currentDirectoryEntries.clear();

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
                result = a.extension().string() < b.extension().string();
                break;
            case SortOption::Size:
                if (!std::filesystem::is_directory(a) && !std::filesystem::is_directory(b)) {
                    result = std::filesystem::file_size(a) < std::filesystem::file_size(b);
                }
                break;
            case SortOption::LastModified:
                result = std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b);
                break;
            }
            return sortAscending ? result : !result;
        });
}

void UIProject::HandleFileSelection(const std::filesystem::path& filePath)
{
    if (filePath.extension() == ".scene") {
        currentSceneFile = filePath.string();
        ImGui::OpenPopup("Load Scene");
    }
    else if (filePath.extension() == ".cs") {
        std::string scriptPath = filePath.string();
        if (!std::filesystem::exists(scriptPath)) {
            LOG(LogType::LOG_ERROR, "Script file does not exist at path: %s", scriptPath.c_str());
        }
        else {
            HINSTANCE result = ShellExecuteA(NULL, "open", scriptPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
            if ((int)result <= 32) {
                LOG(LogType::LOG_ERROR, "Failed to open script file: %s. Error code: %d", scriptPath.c_str(), (int)result);
            }
            else {
                LOG(LogType::LOG_INFO, "Successfully opened script: %s", scriptPath.c_str());
            }
        }
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
        LOG(LogType::LOG_ERROR, "Error renaming item: %s", ex.what());
    }
}

Image* UIProject::GetIconForFile(const std::filesystem::path& filePath)
{
    if (std::filesystem::is_directory(filePath)) {
        return iconCache[".folder"];
    }

    std::string extension = filePath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (iconCache.count(extension)) {
        return iconCache[extension];
    }
    else if (extension == ".wav" || extension == ".ogg" || extension == ".mp3") {
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
        }
        else {
            if (selectedFile.extension() == ".scene" && ImGui::MenuItem("Load without saving")) {
                Application->scene_serializer->DeSerialize(selectedFile.string());
                currentSceneFile = selectedFile.string();
            }
            if (ImGui::MenuItem("Rename")) {
                renamePath = selectedFile;
            }
            if (ImGui::MenuItem("Delete")) {
                try {
                    std::filesystem::remove(selectedFile);
                    selectedFile.clear();
                    StartDirectoryListing(selectedDirectory);
                }
                catch (const std::filesystem::filesystem_error& ex) {
                    LOG(LogType::LOG_ERROR, "Error deleting file: %s", ex.what());
                }
            }

            // Additional file-specific options
            if (selectedFile.extension() == ".cs" && ImGui::MenuItem("Open in Editor")) {
                HandleFileSelection(selectedFile);
            }
        }
    }
}

void UIProject::CreateNewItem(const std::filesystem::path& path, bool isFolder)
{
    isCreatingNewItem = true;
    isNewItemFolder = isFolder;
    newItemPath = path;
    newItemName = isFolder ? "New Folder" : "New File";
}

void UIProject::DrawTruncatedLabel(const std::string& filename)
{
    std::string shortName = filename;
    float textWidth = ImGui::CalcTextSize(shortName.c_str()).x;
    if (textWidth > ICON_SIZE) {
        while (ImGui::CalcTextSize((shortName + "...").c_str()).x > ICON_SIZE && shortName.length() > 3) {
            shortName.pop_back();
        }
        shortName += "...";
    }

    float cursorX = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(cursorX + (ICON_SIZE - ImGui::CalcTextSize(shortName.c_str()).x) * 0.5f);
    ImGui::Text("%s", shortName.c_str());
}