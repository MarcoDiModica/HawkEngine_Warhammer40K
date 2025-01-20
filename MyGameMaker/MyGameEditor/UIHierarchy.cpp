#include "UIHierarchy.h"

#include "App.h"
#include "Root.h"
#include "MyGameEditor/Input.h"
#include "MyGameEngine/GameObject.h"
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

		Scene* currentScene = Application->root->GetActiveScene().get();

		if (currentScene == nullptr) {
			ImGui::Text("No Scene loaded");
		}

		if (currentScene != nullptr) {
			RenderSceneHierarchy(currentScene);
		}

		if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
		{
			ImGui::OpenPopup("HierarchyContextMenu");
		}

		if (ImGui::BeginPopup("HierarchyContextMenu"))
		{
			if (ImGui::MenuItem("Cube")) { Application->root->CreateCube("Cube"); }
			if (ImGui::MenuItem("Sphere")) { Application->root->CreateSphere("Sphere"); }
			if (ImGui::MenuItem("Plane")) { Application->root->CreatePlane("Plane"); }
			if (ImGui::MenuItem("Empty GameObject")) { Application->root->CreateGameObject("Empty"); }
			ImGui::EndPopup();
		}

		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) {
			Application->input->ClearSelection();
		}

		if (draggedObject && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && draggedObject->GetParent()) {
			GameObject* p = draggedObject->GetParent();
			currentScene->AddGameObject(draggedObject->shared_from_this());

			if (p ) p->RemoveChild(draggedObject);
			draggedObject = nullptr;
		}
	}

	ImGui::End();
	return true;
}



void UIHierarchy::RenderSceneHierarchy(Scene* currentScene) {
	//ImGui::Begin("Scene Hierarchy");
	int size = static_cast<int>(Application->root->GetActiveScene()->children().size());

	for (size_t i = 0; i < Application->root->GetActiveScene()->children().size(); ++i) {
		DrawSceneObject(*Application->root->GetActiveScene()->children()[i]);
	}


	//ImGui::End();
}

bool UIHierarchy::DrawSceneObject(GameObject& obj)
{
	bool color = false;
	bool should_continue = true;

	if (obj.isSelected) {
		color = true;
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f)); // Orange color for selected
	}

	bool open = ImGui::TreeNode(obj.GetName().c_str());

	if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1)) {
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
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)  /*&& obj.isSelected*/) {
		if (obj.isSelected) {
			//               type identifier ,  ptr to obj   , size of obj
			ImGui::SetDragDropPayload("GAMEOBJECT", &obj, sizeof(GameObject*));
			/*A payload named "GAMEOBJECT" is created, containing a pointer to obj
			it can be rerieved at a drop target via ImGui::AcceptDragDropPayload */
			int id = obj.GetId();
			ImGui::Text("Dragging %s, gid %d", obj.GetName().c_str(), id); // text created in drag&drop context follows the cursor be default
			draggedObject = &obj;
			//draggedObject->SetName("DraggedFella");
			//ImGui::Text("Dragging %s, gid %d", draggedObject->GetName().c_str(), draggedObject->GetId());
			//RenderSceneHierarchy(Application->root->currentScene.get());
		}
		ImGui::EndDragDropSource(); // Draging context MUST be closed
	}

	if (draggedObject ) {
		bool ImGuiWorks = false; 
		if (ImGui::BeginDragDropTarget()) /*ImGui approach*/ {

			if (draggedObject == &obj) {
				int y = 87;
			}
			ImGuiWorks = true;
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
				
				if (!draggedObject) {
					draggedObject = *(GameObject**)payload->Data;
				}

				Application->root->ParentGameObject(*draggedObject, obj);
				draggedObject = nullptr;
				should_continue = false;
			}
		}/* Approach for when ImGui doesnt work */
		else if (draggedObject && ImGui::IsItemHovered() && Application->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP) {
			if (draggedObject == &obj) {
				int y = 87;
			}
			Application->root->ParentGameObject(*draggedObject, obj);
			draggedObject = nullptr;
			should_continue = false;
		}

		if (ImGuiWorks) { ImGui::EndDragDropTarget(); }

	}

	if (open) {
		for (size_t w = 0; w < obj.GetChildren().size(); ++w) {
			DrawSceneObject(* obj.GetChildren()[w]);
		}
		ImGui::TreePop();
	}

	if (ImGui::BeginPopupContextItem(obj.GetName().c_str()))
	{
		ImGui::Text(obj.GetName().c_str());
		if (ImGui::MenuItem("Delete") && ImGui::IsItemHovered() )
		{
			Application->input->RemoveFromSelection(&obj);
			Application->input->ClearSelection();
			Application->root->RemoveGameObject(&obj);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Empty Child"))
		{
			auto empty = Application->root->CreateGameObject("Empty");
			Application->root->ParentGameObject(*empty, obj);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	return should_continue;
}