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
	ImGuiWindowFlags hierarchyFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Hierarchy", &enabled, hierarchyFlags)) {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowSize(ImVec2(250, 650), ImGuiCond_Once);

		auto& currentScene = Application->root->currentScene;

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


void UIHierarchy::RenderSceneHierarchy(std::shared_ptr<Scene>& currentScene) {
	//ImGui::Begin("Scene Hierarchy");

	for (size_t i = 0; i < Application->root->children.size(); ++i) {
		DrawSceneObject(*Application->root->children[i]);
	}

	//ImGui::End();
}

void UIHierarchy::DrawSceneObject(GameObject& obj)
{
	bool open = ImGui::TreeNode(obj.GetName().c_str());

	if (open) {
		for (auto& child : obj.children()) {
			DrawSceneObject(child);
		}
		ImGui::TreePop();
	}

	ImGui::Button("Delete");

	if (ImGui::IsItemClicked(0)) {
		Application->root->RemoveGameObject(obj.GetName());
	}

	//ImGui::SameLine();
	//if (ImGui::Button("Remove"))
	//{
	//	std::cout << "Remove " << obj.GetName();
	//	Application->root->RemoveGameObject(obj.GetName());
	//}
}