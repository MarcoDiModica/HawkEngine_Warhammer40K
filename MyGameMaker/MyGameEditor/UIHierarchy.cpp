#include "UIHierarchy.h"

#include "App.h"
#include "Root.h"
#include "MyGameEditor/Input.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <SDL2/SDL.h>

UIHierarchy::UIHierarchy(UIType type, std::string name) : UIElement(type, name) {
}

UIHierarchy::~UIHierarchy() {
}

bool UIHierarchy::Draw() {
	ImGuiWindowFlags hierarchyFlags = ImGuiWindowFlags_None;

	if (ImGui::Begin("Hierarchy", &enabled, hierarchyFlags)) {
		ImGuiIO& io = ImGui::GetIO();

		Scene* currentScene = Application->root->currentScene.get();

		if (currentScene == nullptr) {
			ImGui::Text("No Scene loaded");
		}

		if (currentScene != nullptr) {
			RenderSceneHierarchy(currentScene);
		}

		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) {
			Application->input->ClearSelection();
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
		if (Application->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) {
			if (obj.isSelected) {
				Application->input->RemoveFromSelection(&obj);
			}
			else {
				Application->input->AddToSelection(&obj);
			}
		}
		else {
			Application->input->ClearSelection();
			Application->input->AddToSelection(&obj);
		}
	}

	if (obj.isSelected && color) {
		ImGui::PopStyleColor(); // Orange color for selected
	}

	// IF the treenode is dragged
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		//               type identifier ,  ptr to obj   , size of obj
		ImGui::SetDragDropPayload("GAMEOBJECT", &obj, sizeof(GameObject*));
		/*A payload named "GAMEOBJECT" is created, containing a pointer to obj
		it can be rerieved at a drop target via ImGui::AcceptDragDropPayload */

		ImGui::Text("Dragging %s", obj.GetName().c_str()); // text created in drag&drop context follows the cursor be default

		ImGui::EndDragDropSource(); // Draging context MUST be closed
	}

	// Drag-and-Drop: Target
	if (ImGui::BeginDragDropTarget()) {
		// Retrieve playload, ptr to dragged obj
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {

			GameObject* draggedObj = *(GameObject**)payload->Data;
			if (draggedObj && draggedObj != &obj) {

				std::cout << "dragged " << draggedObj->GetName() << "into " << obj.GetName();
				//obj.emplaceChild(*draggedObj);
				Application->root->ParentGameObject(*draggedObj, obj);
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (open) {
		for (auto& child : obj.children()) {
			DrawSceneObject(child);
		}
		ImGui::TreePop();
	}

	if (ImGui::BeginPopupContextItem(obj.GetName().c_str()))
	{
		if (ImGui::MenuItem("Delete"))
		{
			Application->root->RemoveGameObject(&obj);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}