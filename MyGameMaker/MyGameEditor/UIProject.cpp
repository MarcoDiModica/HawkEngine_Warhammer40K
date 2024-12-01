#include "UIProject.h"
#include "App.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "MyGUI.h"
#include "../MyGameEngine/types.h"

#include <filesystem>
#include <string>

#define BIT(x) (1 << x)

UIProject::UIProject(UIType type, std::string name) : UIElement(type, name)
{
	// Definir el path del directorio (cambiar en types.h)
	directoryPath = ASSETS_PATH;
	currentSceneFile = "";
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
		int width = ImGui::GetContentRegionAvail().x;

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

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

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

		std::string name = entry.path().string();

		auto lastMalcomX = name.find_last_of("/\\");
		lastMalcomX = lastMalcomX == std::string::npos ? 0 : lastMalcomX + 1;
		name = name.substr(lastMalcomX, name.size() - lastMalcomX);

		bool entryIsFile = !std::filesystem::is_directory(entry.path());
		if (entryIsFile)
		{
			treeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)(*count), treeFlags, name.c_str());
	
		if (ImGui::IsItemClicked())
		{
			nodeClicked = *count;
			anyNodeClicked = true;
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
			else
			{
				for (const auto& e : std::filesystem::recursive_directory_iterator(entry.path()))
				{
					(*count)--;
				}
			}
		}

		if (entryIsFile && name.ends_with(".scene"))
		{
			if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(0))
			{
				ImGui::OpenPopup("Cargar escena");
			}

			if (ImGui::BeginPopupModal("Cargar escena"))
			{
				if (ImGui::Button("Guardar y cargar"))
				{
					Application->scene_serializer->Serialize(path.string());
					Application->scene_serializer->DeSerialize(entry.path().string());
					currentSceneFile = entry.path().string();
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Cargar sin guardar"))
				{
					Application->scene_serializer->DeSerialize(entry.path().string());
					currentSceneFile = entry.path().string();
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("No cargar"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
	}

	return { anyNodeClicked, nodeClicked };
}



