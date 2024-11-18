#include "UIHierarchy.h"

#include "App.h"
#include "Root.h"
#include "MyGameEditor/Input.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>

UIHierarchy::UIHierarchy(UIType type, std::string name) : UIElement(type, name) {
}

UIHierarchy::~UIHierarchy() {
}

bool UIHierarchy::Draw() {


	ImGuiWindowFlags hierarchyFlags = ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_AlwaysAutoResize;

	ImVec2 hierarchyPos = ImVec2(0, 30);
	ImGui::SetNextWindowPos(hierarchyPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(250, 650), ImGuiCond_Once);

	if (ImGui::Begin("Hierarchy", &enabled, hierarchyFlags)) {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowSize(ImVec2(250, 650), ImGuiCond_Once);

		Scene* currentScene = Application->root->currentScene.get();

		if (currentScene == nullptr) {
			ImGui::Text("No Scene loaded");
		}

		if (currentScene != nullptr) {
			RenderSceneHierarchy(currentScene);
		}
	}

	ImGui::End();
	return true;
}


void UIHierarchy::RenderSceneHierarchy(Scene* currentScene) {
	//ImGui::Begin("Scene Hierarchy");

	for (size_t i = 0; i < Application->root->currentScene->children().size(); ++i) {
		DrawSceneObject(*Application->root->currentScene->children()[i]);
	}

	//ImGui::End();
}

void UIHierarchy::DrawSceneObject(GameObject& obj)
{
	bool color = false;

	if (obj.isSelected) {
		color = true;
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f)); // Orange color for selected
	}

	bool open = ImGui::TreeNode(obj.GetName().c_str());
	if (ImGui::IsItemClicked(0)) {
		Application->input->SetSelectedGameObject(&obj);
	}

	if (obj.isSelected && color) {
		ImGui::PopStyleColor(); // Orange color for selected
	}
	

	if (open) {
		for (auto& child : obj.children()) {
			DrawSceneObject(child);
		}
		ImGui::TreePop();
	}


	ImGui::Button("Delete");

	if (ImGui::IsItemClicked(0)) {

		obj.Destroy();
	}



	//ImGui::SameLine();
	//if (ImGui::Button("Remove"))
	//{
	//	std::cout << "Remove " << obj.GetName();
	//	Application->root->RemoveGameObject(obj.GetName());
	//}
}