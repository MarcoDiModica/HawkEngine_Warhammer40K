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
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGuiWindowFlags hierarchyFlags = ImGuiWindowFlags_None;

	if (ImGui::Begin("Hierarchy", &enabled, hierarchyFlags)) {

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
	ImGui::PopStyleColor();
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
    bool useGray = (obj.GetId() % 2 == 0); // Toggle for alternating colors
    bool should_continue = true;

    // Alternate the text color
    ImVec4 textColor = useGray
        ? ImVec4(0.6f, 0.6f, 0.6f, 1.0f)  // Gray
        : ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Dark gray

    if (obj.isSelected) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.53f, 0.81f, 0.92f, 1.0f)); // Blue for selected
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Text, textColor); // Apply alternating colors
    }

    // Draw the tree node
    bool open = ImGui::TreeNode(obj.GetName().c_str());

    // Selection logic
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

    ImGui::PopStyleColor(); // Restore the original text color

    // Drag & drop logic
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        if (obj.isSelected) {
            ImGui::SetDragDropPayload("GAMEOBJECT", &obj, sizeof(GameObject*));
            int id = obj.GetId();
            ImGui::Text("Dragging %s, gid %d", obj.GetName().c_str(), id);
            draggedObject = &obj;
        }
        ImGui::EndDragDropSource();
    }

    if (draggedObject) {
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
                if (!draggedObject) {
                    draggedObject = *(GameObject**)payload->Data;
                }
                Application->root->ParentGameObject(*draggedObject, obj);
                draggedObject = nullptr;
                should_continue = false;
            }
            ImGui::EndDragDropTarget();
        }
    }

    // Recursively draw children
    if (open) {
        for (size_t w = 0; w < obj.GetChildren().size(); ++w) {
            DrawSceneObject(*obj.GetChildren()[w]);
        }
        ImGui::TreePop();
    }

    // Context menu logic
    if (ImGui::BeginPopupContextItem(obj.GetName().c_str())) {
        ImGui::Text(obj.GetName().c_str());
        if (ImGui::MenuItem("Delete") && ImGui::IsItemHovered()) {
            Application->input->RemoveFromSelection(&obj);
            Application->input->ClearSelection();
            Application->root->RemoveGameObject(&obj);
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Empty Child")) {
            auto empty = Application->root->CreateGameObject("Empty");
            Application->root->ParentGameObject(*empty, obj);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    return should_continue;
}
