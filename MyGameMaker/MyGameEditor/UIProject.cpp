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
	directoryPath = ASSETS_PATH;
	currentSceneFile = "";

	folderIcon = new Image();
	folderIcon->LoadTexture("Assets/Icons/folder_icon.png");


	fbxIcon = new Image();
	//fbxIcon->LoadTexture("Assets/Icons/fbx_icon.png");
	fbxIcon->LoadTexture("Assets/Icons/folder_icon.png");

	pngIcon = new Image();
	//pngIcon->LoadTexture("Assets/Icons/png_icon.png");
	pngIcon->LoadTexture("Assets/Icons/folder_icon.png");

	sceneIcon = new Image();
	//sceneIcon->LoadTexture("Assets/Icons/scene_icon.png");
	sceneIcon->LoadTexture("Assets/Icons/folder_icon.png");

	meshIcon = new Image();
	//meshIcon->LoadTexture("Assets/Icons/mesh_icon.png");
	meshIcon->LoadTexture("Assets/Icons/folder_icon.png");
}

UIProject::~UIProject()
{
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

	if (ImGui::Begin("Project", &enabled, projectFlags))
	{
		static ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;

		if (ImGui::BeginTable("Table", 2, tableFlags))
		{
			ImGui::TableNextColumn();
			if (ImGui::CollapsingHeader("Assets"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				uint32_t count = 0;
				// Esto no sé si está bien
				for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
				{
					count++;
				}

				static int selectionMask = 0;

				auto clickState = DirectoryView(directoryPath, &count, &selectionMask);

				if (clickState.first) // Esto es para la selección múltiple
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

			// Aquí creo que va algo
			ImGui::Text("Properties or Preview here...");
			ImGui::EndTable();
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}

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

			if (extension == ".fbx" || extension == ".FBX")
				icon = fbxIcon;
			else if (extension == ".png")
				icon = pngIcon;
			else if (extension == ".scene")
				icon = sceneIcon;
			else if (extension == ".mesh")
				icon = meshIcon;

			treeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		if (icon)
		{
			ImGui::Image(reinterpret_cast<void*>(icon->id()), ImVec2(16, 16));
			ImGui::SameLine();
		}

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)(*count), treeFlags, name.c_str());

		if (ImGui::IsItemClicked())
		{
			nodeClicked = *count;
			anyNodeClicked = true;
			HandleFileSelection(entry.path().string());
		}

		if (ImGui::BeginPopupContextItem(name.c_str()))
		{
			if (name.ends_with(".scene"))
			{
				if (ImGui::MenuItem("Save & Load Scene"))
				{
					Application->scene_serializer->Serialize(currentSceneFile);
					Application->scene_serializer->DeSerialize(entry.path().string());
					currentSceneFile = entry.path().string();
				}
				if (ImGui::MenuItem("Load Scene without saving"))
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
		if (ImGui::Button("Save & Load"))
		{
			Application->scene_serializer->Serialize(currentSceneFile);
			Application->scene_serializer->DeSerialize(filePath);
			currentSceneFile = filePath;
			ImGui::CloseCurrentPopup();
		}

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





