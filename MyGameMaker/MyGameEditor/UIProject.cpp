#include "UIProject.h"
#include "App.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "MyGUI.h"
#include "../MyGameEngine/types.h"

#include <filesystem>
#include <string>
#include <fstream>

#define BIT(x) (1 << x)

UIProject::UIProject(UIType type, std::string name) : UIElement(type, name)
{
	// Definir el path del directorio (cambiar en types.h)
	directoryPath = LIBRARY_PATH;
	selectedDirectory = directoryPath;
	currentSceneFile = "";

	folderIcon = new Image();
	folderIcon->LoadTexture("EngineAssets/folder.png");

	matIcon = new Image();
	//fbxIcon->LoadTexture("Assets/Icons/fbx_icon.png");
	matIcon->LoadTexture("EngineAssets/material.png");

	imageIcon = new Image();
	//pngIcon->LoadTexture("Assets/Icons/png_icon.png");
	imageIcon->LoadTexture("EngineAssets/image.png");

	sceneIcon = new Image();
	//sceneIcon->LoadTexture("Assets/Icons/scene_icon.png");
	sceneIcon->LoadTexture("EngineAssets/scene.png");

	meshIcon = new Image();
	//meshIcon->LoadTexture("Assets/Icons/mesh_icon.png");

	meshIcon->LoadTexture("EngineAssets/mesh.png");

	audioIcon = new Image();
	audioIcon->LoadTexture("Assets/Icons/folder_icon.png");

}

UIProject::~UIProject()
{
	delete folderIcon;
	delete matIcon;
	delete imageIcon;
	delete sceneIcon;
	delete meshIcon;
	delete audioIcon;
}

bool UIProject::Draw()
{
	ImGuiWindowFlags projectFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_None;
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

	ImGuiDockNode* dockNode = ImGui::GetWindowDockNode();
	bool isDocked = (dockNode != nullptr);

	bool isMinimized = (isDocked && (dockNode->Size.x <= 0.0f || dockNode->Size.y <= 0.0f));

	if (isMinimized || !windowActive)
	{
		ImGui::End();
		ImGui::PopStyleVar();
		return true;
	}

	static ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;

	if (ImGui::BeginTable("Table", 2, tableFlags))
	{
		ImGui::TableNextColumn();
		if (ImGui::CollapsingHeader("Assets"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			uint32_t count = 0;

			// Esto no s� si est� bien

			for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
			{
				count++;
			}

			static int selectionMask = 0;

			auto clickState = DirectoryView(directoryPath, &count, &selectionMask);

			if (clickState.first) // Esto es para la selecci�n m�ltiple
			{
				if (ImGui::GetIO().KeyCtrl)
				{
					selectionMask ^= BIT(clickState.second);
				}
				else
				{
					selectionMask = BIT(clickState.second);
				}
			}
		}

		ImGui::TableNextColumn();


		// Aqu� creo que va algo
		ImGui::Text("Properties or Preview here...");

		if (selectedDirectory.empty())
		{
			ImGui::Text("Select a folder to view contents.");
		}
		else
		{
			DrawFolderContents(selectedDirectory);
		}

		ImGui::EndTable();
	}

	ImGui::PopStyleVar();
	ImGui::End();

	return true;
}

std::pair<bool, uint32_t> UIProject::DirectoryView(const std::filesystem::path& path, uint32_t* count, int* selection_mask)
{
	std::pair<bool, uint32_t> ret = { false, 0 };

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	bool anyNodeClicked = false;
	uint32_t nodeClicked = 0;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		ImGuiTreeNodeFlags treeFlags = nodeFlags;
		const bool isSelected = (*selection_mask & BIT(*count)) != 0;
		if (isSelected)
		{
			treeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		std::string name = entry.path().filename().string();
		bool entryIsFile = !std::filesystem::is_directory(entry.path());
		auto icon = entryIsFile ? nullptr : folderIcon;
		if (entryIsFile)
		{
			std::string extension = entry.path().extension().string();

			if (extension == ".mat" || extension == ".MAT")
				icon = matIcon;
			else if (extension == ".image")
				icon = imageIcon;
			else if (extension == ".scene")
				icon = sceneIcon;
			else if (extension == ".mesh")
				icon = meshIcon;
			else if (extension == ".wav" || extension == ".ogg" || extension == ".mp3")
				icon = audioIcon;

			treeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		if (icon)
		{
			ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(icon->id())), ImVec2(16, 16));
			ImGui::SameLine();
		}

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)(*count), treeFlags, name.c_str());

		if (!entryIsFile && ImGui::IsItemClicked())
		{
			selectedDirectory = entry.path();
			nodeClicked = *count;
			anyNodeClicked = true;
			HandleFileSelection(entry.path().string());
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			std::string fullPath = entry.path().string();
			ImGui::SetDragDropPayload("ASSET_PATH", fullPath.c_str(), fullPath.length() + 1);
			ImGui::Text("Dragging %s", name.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginPopupContextItem(name.c_str()))
		{
			if (name.ends_with(".scene"))
			{
				/*if (ImGui::MenuItem("Save & Load"))
				{
					Application->scene_serializer->Serialize(currentSceneFile);
					Application->scene_serializer->DeSerialize(entry.path().string());
					currentSceneFile = entry.path().string();
				}*/
				if (ImGui::MenuItem("Load without saving"))
				{
					Application->scene_serializer->DeSerialize(entry.path().string());
					currentSceneFile = entry.path().string();
				}
			}

			if (ImGui::MenuItem("Delete"))
			{
				std::filesystem::remove_all(entry.path());
			}

			ImGui::EndPopup();
		}
		(*count)--;
		if (!entryIsFile)
		{
			if (nodeOpen)
			{
				auto clickState = DirectoryView(entry.path(), count, selection_mask);
				if (!anyNodeClicked)
				{
					anyNodeClicked = clickState.first;
					nodeClicked = clickState.second;
				}

				ImGui::TreePop();
			}
		}

	}

	return { anyNodeClicked, nodeClicked };
}

void UIProject::HandleFileSelection(const std::string& filePath)
{
	if (filePath.ends_with(".scene"))
	{
		currentSceneFile = filePath;
		ImGui::OpenPopup(("Load Scene"));
	}

	if (ImGui::BeginPopupModal("Load Scene"))
	{
		if (ImGui::Button("Load without saving"))
		{
			Application->scene_serializer->DeSerialize(filePath);
			currentSceneFile = filePath;
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void UIProject::DrawFolderContents(const std::filesystem::path& folderPath) {
	if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath)) {
		return;
	}
	// Get the width of the content region
	float contentRegionWidth = ImGui::GetContentRegionAvail().x;
	// Icon and padding sizes
	const float iconSize = 64.0f;
	const float padding = 16.0f;  // Padding between icons
	const float totalSize = iconSize + padding;  // Total space per item
	// Calculate how many icons fit per row
	int itemsPerRow = static_cast<int>(contentRegionWidth / totalSize);
	if (itemsPerRow < 1) itemsPerRow = 1;  // At least one item per row
	int itemIndex = 0;

	for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
		bool isDirectory = std::filesystem::is_directory(entry.path());
		auto icon = isDirectory ? folderIcon : nullptr;

		// Determine the icon based on file type
		if (!isDirectory) {
			std::string extension = entry.path().extension().string();
			if (extension == ".mat" || extension == ".MAT") {
				icon = matIcon;
			}
			else if (extension == ".image") {
				icon = imageIcon;
			}
			else if (extension == ".scene") {
				icon = sceneIcon;
			}
			else if (extension == ".mesh") {
				icon = meshIcon;
			}
		}
		if (icon) {
			ImGui::BeginGroup();
			// Display the icon as a button
			if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(icon->id())), ImVec2(iconSize, iconSize))) {
				if (isDirectory) {
					selectedDirectory = entry.path();  // Update the selected directory
				}
				else {
					HandleFileSelection(entry.path().string());  // Handle file clicks
				}
			}
			// Drag-and-drop support
			if (ImGui::IsItemHovered() && ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload("ASSET_PATH", entry.path().string().c_str(), entry.path().string().length() + 1);
				ImGui::Text("Dragging: %s", entry.path().filename().string().c_str());
				ImGui::EndDragDropSource();
			}
			// Constrain text to the icon width
			float textWidth = iconSize;  // Confine the text to the icon size
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + textWidth);

			// Display the file/folder name (truncate if necessary)
			std::string filename = entry.path().filename().string();
			if (filename.size() > 15) {  // Truncate long names with ellipses
				filename = filename.substr(0, 12) + "...";
			}
			ImGui::TextWrapped("%s", filename.c_str());
			ImGui::PopTextWrapPos();  // Restore text wrapping
			ImGui::EndGroup();

			// Move to the next column or row
			itemIndex++;
			if (itemIndex % itemsPerRow != 0) {
				ImGui::SameLine();  // Keep items on the same row
			}
		}
	}
}