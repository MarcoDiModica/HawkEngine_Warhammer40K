// UIProject.cpp
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
    iconCache[".default"] = new Image();
    iconCache[".default"]->LoadTexture(DEFAULT_ICON_PATH);

    currentSortOption = SortOption::Name;
    sortAscending = true;

    StartDirectoryListing(directoryPath);
}

UIProject::~UIProject()
{
    for (auto& pair : iconCache) {
        delete pair.second;
    }
    iconCache.clear();
}

bool UIProject::Draw()
{
    ImGuiWindowFlags projectFlags = ImGuiWindowFlags_NoCollapse;
    ImGuiWindowClass windowClass;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    if (firstDraw)
    {
        ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
        firstDraw = false;
    }

    ImGui::SetNextWindowClass(&windowClass);
    windowClass.DockingAllowUnclassed = false;

    bool windowActive = ImGui::Begin("Library", &enabled, projectFlags);

    if (!windowActive)
    {
        ImGui::End();
        ImGui::PopStyleVar();
        return true;
    }

    static ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;

    if (ImGui::BeginTable("MainTable", 2, tableFlags))
    {
        ImGui::TableSetupColumn("DirectoryTree", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        DrawDirectoryTree();

        ImGui::TableSetColumnIndex(1);
        DrawBreadcrumbs();

        ImGui::TableSetColumnIndex(1);

        // --- Buttons ---
        if (selectedDirectory != directoryPath) {
            if (ImGui::Button("Back")) {
                selectedDirectory = selectedDirectory.parent_path();
                selectedFile.clear();
                StartDirectoryListing(selectedDirectory);
            }
            ImGui::SameLine();
        }

        if (ImGui::Button("Refresh"))
        {
            StartDirectoryListing(selectedDirectory);
        }
        ImGui::SameLine();

        if (ImGui::Button("Create CS Script"))
		{
            //ask for script name
			std::string scriptName = "NewScript";
            std::string scriptPath = selectedDirectory.string() + scriptName + ".cs";
            std::ofstream scriptFile(scriptPath);
            
            std::string scriptContent = "using System;\n\n"
                "public class " + scriptName + " : MonoBehaviour\n"
                "{\n"
                "    public override void Start()\n"
                "    {\n"
                "        HawkEngine.EngineCalls.print(\"Hola desde " + scriptName + "!\");\n"
                "    }\n\n"
                "    public override void Update(float deltaTime)\n"
                "    {\n"
                "        // L�gica de actualizaci�n\n"
                "    }\n"
                "}\n";

            scriptFile << scriptContent;
            scriptFile.close();

            StartDirectoryListing(selectedDirectory);
		}

        if (ImGui::Button("Create")) {
            ImGui::OpenPopup("CreateMenu");
        }

        if (ImGui::BeginPopup("CreateMenu")) {
            if (ImGui::MenuItem("Folder")) {
                ImGui::OpenPopup("CreateNewFolder");
            }
            if (ImGui::MenuItem("File")) {
                ImGui::OpenPopup("CreateNewFile");
            }
            ImGui::EndPopup();
        }

        // --- Folder Contents ---
        ImGui::TableSetColumnIndex(1);
        ImGui::Separator();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        DrawFolderContents(selectedDirectory);

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("ContextMenu");
        }

        if (ImGui::BeginPopup("ContextMenu")) {
            ShowContextMenu();
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();

        ImGui::EndTable();
    }

    if (ImGui::BeginPopupModal("CreateNewFolder")) {
        static char newFolderName[256] = "";
        ImGui::InputText("Folder Name", newFolderName, IM_ARRAYSIZE(newFolderName));
        if (ImGui::Button("Create")) {
            CreateNewItem(selectedDirectory, newFolderName, true);
            ImGui::CloseCurrentPopup();
            newFolderName[0] = '\0';
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
            newFolderName[0] = '\0';
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("CreateNewFile"))
    {
        static char newFileName[256] = "";
        ImGui::InputText("File Name", newFileName, IM_ARRAYSIZE(newFileName));
        if (ImGui::Button("Create"))
        {
            CreateNewItem(selectedDirectory, newFileName, false);
            ImGui::CloseCurrentPopup();
            newFileName[0] = '\0';
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
            newFileName[0] = '\0';
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Rename Item")) {
        static char newName[256] = "";
        std::filesystem::path originalPath;

        if (std::filesystem::is_directory(renamePath)) {
            originalPath = renamePath;
            strncpy(newName, renamePath.filename().string().c_str(), sizeof(newName) - 1);
        }
        else {
            originalPath = renamePath;
            strncpy(newName, renamePath.filename().string().c_str(), sizeof(newName) - 1);
        }
        newName[sizeof(newName) - 1] = '\0';

        ImGui::InputText("New Name", newName, IM_ARRAYSIZE(newName));

        if (ImGui::Button("Rename")) {
            try {
                std::filesystem::path newPath = originalPath.parent_path() / newName;
                std::filesystem::rename(originalPath, newPath);
                if (selectedFile == originalPath) {
                    selectedFile = newPath;
                }
                StartDirectoryListing(selectedDirectory);
                ImGui::CloseCurrentPopup();
                newName[0] = '\0';
            }
            catch (const std::filesystem::filesystem_error& ex) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error renaming item: %s", ex.what());
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
            newName[0] = '\0';
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    ImGui::End();

    return true;
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

void UIProject::StartDirectoryListing(const std::filesystem::path& path)
{
    isLoading = true;
    currentDirectoryEntries.clear();

    directoryListingFuture = std::async(std::launch::async, [path]() {
        std::vector<std::filesystem::path> entries;
        try
        {
            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                entries.push_back(entry.path());
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error listing directory: " << e.what() << std::endl;
        }
        return entries;
        });
}

void UIProject::DrawFolderContents(const std::filesystem::path& path)
{
    static char filter[256] = "";
    ImGui::InputText("Filter", filter, IM_ARRAYSIZE(filter));

    if (isLoading)
    {
        if (directoryListingFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
        {
            currentDirectoryEntries = directoryListingFuture.get();
            isLoading = false;
            std::sort(currentDirectoryEntries.begin(), currentDirectoryEntries.end(),
                [&](const std::filesystem::path& a, const std::filesystem::path& b) {
                    bool result = false;
                    switch (currentSortOption) {
                    case SortOption::Name:
                        result = a.filename().string() < b.filename().string();
                        break;
                    case SortOption::Type:
                        result = a.extension().string() < b.extension().string();
                        break;
                    case SortOption::Size:
                        if (!std::filesystem::is_directory(a) && !std::filesystem::is_directory(b))
                        {
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
        else
        {
            ImGui::Text("Loading...");
            return;
        }
    }

    if (ImGui::Button("Sort"))
    {
        ImGui::OpenPopup("SortOptions");
    }
    if (ImGui::BeginPopup("SortOptions")) {
        if (ImGui::Selectable("Name", currentSortOption == SortOption::Name))
            currentSortOption = SortOption::Name;
        if (ImGui::Selectable("Type", currentSortOption == SortOption::Type))
            currentSortOption = SortOption::Type;
        if (ImGui::Selectable("Size", currentSortOption == SortOption::Size))
            currentSortOption = SortOption::Size;
        if (ImGui::Selectable("Last Modified", currentSortOption == SortOption::LastModified))
            currentSortOption = SortOption::LastModified;

        ImGui::Separator();
        if (ImGui::Checkbox("Ascending", &sortAscending)) {

        }

        ImGui::EndPopup();
    }

    float contentRegionWidth = ImGui::GetContentRegionAvail().x;
    const float iconSize = 64.0f;
    const float padding = 16.0f;
    const float totalSize = iconSize + padding;
    int itemsPerRow = static_cast<int>(std::max(1.0f, contentRegionWidth / totalSize));
    int itemIndex = 0;

    try
    {
        for (const auto& entry : currentDirectoryEntries)
        {
            std::string filename = entry.filename().string();

            if (filter[0] != '\0' && filename.find(filter) == std::string::npos) {
                continue;
            }

            Image* icon = GetIconForFile(entry);

            ImGui::PushID(filename.c_str());

            ImGui::BeginGroup();

            float cursorX = ImGui::GetCursorPosX();
            float textWidth = ImGui::CalcTextSize(filename.c_str()).x;
            float offset = (totalSize - std::min(textWidth, iconSize)) * 0.5f;
            ImGui::SetCursorPosX(cursorX + offset);

            if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(icon->id())), ImVec2(iconSize, iconSize)))
            {
                selectedFile = entry;
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                selectedFile = entry;
            }

            if (ImGui::IsItemHovered())
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    if (std::filesystem::is_directory(entry))
                    {
                        selectedDirectory = entry;
                        selectedFile.clear();
                        StartDirectoryListing(selectedDirectory);
                    }
                    else
                    {
                        HandleFileSelection(entry);
                    }
                }

                ImGui::BeginTooltip();
                ImGui::Text("%s", filename.c_str());
                ImGui::EndTooltip();
            }

            if (ImGui::BeginDragDropSource())
            {
                std::string fullPath = entry.string();
                ImGui::SetDragDropPayload("ASSET_PATH", fullPath.c_str(), fullPath.length() + 1);
                ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(icon->id())), ImVec2(iconSize / 2, iconSize / 2));
                ImGui::Text("%s", filename.c_str());
                ImGui::EndDragDropSource();
            }

            ImGui::SetCursorPosX(cursorX + offset);
            std::string shortName = filename;
            if (ImGui::CalcTextSize(shortName.c_str()).x > iconSize)
            {
                while (ImGui::CalcTextSize((shortName + "...").c_str()).x > iconSize && shortName.length() > 3)
                {
                    shortName.pop_back();
                }
                shortName += "...";
            }

            ImGui::Text("%s", shortName.c_str());

            ImGui::EndGroup();

            itemIndex++;
            if (itemIndex % itemsPerRow != 0)
            {
                ImGui::SameLine();
            }

            ImGui::PopID();
        }
    }
    catch (const std::filesystem::filesystem_error& ex)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", ex.what());
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

void UIProject::HandleFileSelection(const std::filesystem::path& filePath)
{
    if (filePath.extension() == ".scene") {
        currentSceneFile = filePath.string();

        ImGui::OpenPopup("Load Scene");
    }

    if (filePath.extension() == ".cs") 
    {
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

    if (ImGui::BeginPopupModal("Load Scene"))
    {
        if (ImGui::Button("Load without saving"))
        {
            Application->scene_serializer->DeSerialize(filePath.string());
            currentSceneFile = filePath.string();
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
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
    else if (extension == ".wav" || extension == ".ogg" || extension == ".mp3")
    {
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
            ImGui::OpenPopup("CreateNewFolder");
        }
        if (ImGui::MenuItem("New File")) {
            ImGui::OpenPopup("CreateNewFile");
        }
    }
    else
    {
        if (std::filesystem::is_directory(selectedFile))
        {
            if (ImGui::MenuItem("Open"))
            {
                selectedDirectory = selectedFile;
                selectedFile.clear();
                StartDirectoryListing(selectedDirectory);
            }
            if (ImGui::MenuItem("Rename"))
            {
                renamePath = selectedFile;
                ImGui::OpenPopup("Rename Item");
            }

            if (ImGui::MenuItem("Delete Folder"))
            {
                try {
                    std::filesystem::remove_all(selectedFile);
                    selectedFile.clear();
                    if (selectedDirectory == selectedFile)
                    {
                        selectedDirectory = directoryPath;
                    }
                    StartDirectoryListing(selectedDirectory);
                }
                catch (const std::filesystem::filesystem_error& ex) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error deleting folder: %s", ex.what());
                }
            }
        }
        else
        {
            if (selectedFile.extension() == ".scene")
            {
                if (ImGui::MenuItem("Load without saving"))
                {
                    Application->scene_serializer->DeSerialize(selectedFile.string());
                    currentSceneFile = selectedFile.string();

                }
            }
            if (ImGui::MenuItem("Rename")) {
                renamePath = selectedFile;
                ImGui::OpenPopup("Rename Item");
            }
            if (ImGui::MenuItem("Delete File"))
            {
                try
                {
                    std::filesystem::remove(selectedFile);
                    selectedFile.clear();
                    StartDirectoryListing(selectedDirectory);
                }
                catch (const std::filesystem::filesystem_error& ex)
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error deleting file: %s", ex.what());
                }
            }
        }
    }
}

void UIProject::CreateNewItem(const std::filesystem::path& path, const std::string& itemName, bool isDirectory)
{
    try {
        if (isDirectory) {
            std::filesystem::create_directory(path / itemName);
        }
        else {
            std::ofstream newFile(path / itemName);
        }
        StartDirectoryListing(selectedDirectory);
    }
    catch (const std::filesystem::filesystem_error& ex) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", ex.what());
    }
}