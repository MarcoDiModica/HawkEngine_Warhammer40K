#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <SDL2/SDL.h>

#include "UIHierarchy.h"
#include "App.h"
#include "Root.h"
#include "MyGameEditor/Input.h"
#include "MyGameEngine/GameObject.h"

UIHierarchy::UIHierarchy(UIType type, std::string name) : UIElement(type, name) {
}

UIHierarchy::~UIHierarchy() {
    delete draggedObject;
    draggedObject = nullptr;
}

bool UIHierarchy::Draw() {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    ImGuiWindowFlags hierarchyFlags = ImGuiWindowFlags_None;

    if (ImGui::Begin("Hierarchy", &enabled, hierarchyFlags)) {

        Scene* currentScene = Application->root->GetActiveScene().get();

		if (currentScene == nullptr) {
			ImGui::Text("No Scene loaded");
		}
		else {
			static bool renamingScene = false;
			static std::string currentSceneName;

			if (!renamingScene) {
				ImGui::Text(currentScene->GetName().c_str());

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					renamingScene = true;
					currentSceneName = currentScene->GetName();
				}
			}
			else {
				currentSceneName.resize(100);

				if (ImGui::InputText("##RenameScene", currentSceneName.data(), 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
					currentSceneName.resize(strlen(currentSceneName.data()));
					if (currentSceneName.length() <= 100) {
						currentScene->SetName(currentSceneName);
					}
					else {
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Scene name too long!");
					}
					renamingScene = false;
				}
			}
		}

        ImGui::Separator();

        if (currentScene != nullptr) {
            RenderSceneHierarchy(currentScene);
        }

        if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
        {
            ImGui::OpenPopup("HierarchyContextMenu");
        }

        if (ImGui::BeginPopup("HierarchyContextMenu"))
        {
            if (ImGui::MenuItem("Empty GameObject")) { Application->root->CreateGameObject("Empty"); }
            if (ImGui::BeginMenu("3D Objects"))
			{
				if (ImGui::MenuItem("Cube")) { Application->root->CreateCube("Cube"); }
				if (ImGui::MenuItem("Sphere")) { Application->root->CreateSphere("Sphere"); }
				if (ImGui::MenuItem("Cylinder")) { Application->root->CreateCylinder("Cylinder"); }
                if (ImGui::MenuItem("Plane")) { Application->root->CreatePlane("Plane"); }
				//if (ImGui::MenuItem("Cone")) { Application->root->CreateCone("Cone"); }
				//if (ImGui::MenuItem("Torus")) { Application->root->CreateTorus("Torus"); }
				ImGui::EndMenu();
			}
            if (ImGui::BeginMenu("Lights"))
            {
                if (ImGui::MenuItem("Point Light")) { Application->root->CreateLightObject("Light"); }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Cameras"))
			{
				if (ImGui::MenuItem("Camera")) { Application->root->CreateCameraObject("Camera"); }
				ImGui::EndMenu();
			}
            ImGui::Separator();
            if (ImGui::MenuItem("Save Scene")) { Application->scene_serializer->Serialize("Library/Scenes"); }

            ImGui::EndPopup();
        }

        if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) {
            Application->input->ClearSelection();
        }

        if (draggedObject && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && draggedObject->GetParent()) {
            GameObject* p = draggedObject->GetParent();
            currentScene->AddGameObject(draggedObject->shared_from_this());

            if (p) p->RemoveChild(draggedObject);
            draggedObject = nullptr;
        }
    }

    ImGui::End();
    ImGui::PopStyleColor();
    return true;
}

void UIHierarchy::RenderSceneHierarchy(Scene* currentScene) {
    int size = static_cast<int>(Application->root->GetActiveScene()->children().size());

    for (size_t i = 0; i < Application->root->GetActiveScene()->children().size(); ++i) {
        DrawSceneObject(*Application->root->GetActiveScene()->children()[i]);
    }
}

bool UIHierarchy::DrawSceneObject(GameObject& obj)
{
	bool useGray = (obj.GetId() % 2 == 0);
	bool should_continue = true;

	bool hasChildren = !obj.GetChildren().empty();

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (!hasChildren) {
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (obj.isSelected) {
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.53f, 0.81f, 0.92f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.4f, 0.6f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.5f, 0.7f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.4f, 0.6f, 0.8f, 0.7f));
	}
	else {
		bool isChildOfSelected = false;
		GameObject* parent = obj.GetParent();
		if (parent && parent->isSelected) {
			isChildOfSelected = true;
		}

		if (isChildOfSelected) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.8f, 0.9f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.15f, 0.25f, 0.35f, 0.3f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.2f, 0.3f, 0.4f, 0.4f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.25f, 0.35f, 0.45f, 0.5f));
		}
		else {
			ImVec4 textColor = useGray
				? ImVec4(0.7f, 0.7f, 0.7f, 1.0f)
				: ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.15f, 0.15f, 0.15f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.25f, 0.25f, 0.25f, 0.6f));
		}
	}

	bool open = ImGui::TreeNodeEx(obj.GetName().c_str(), nodeFlags);

	if (ImGui::IsItemClicked(0) && !ImGui::IsItemToggledOpen()) {
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

	ImGui::PopStyleColor(4);

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		ImGui::SetDragDropPayload("GAMEOBJECT", &obj, sizeof(GameObject*));
		int id = obj.GetId();
		ImGui::Text("Dragging %s, gid %d", obj.GetName().c_str(), id);
		draggedObject = &obj;
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

	if (open) {
		for (size_t w = 0; w < obj.GetChildren().size(); ++w) {
			DrawSceneObject(*obj.GetChildren()[w]);
		}
		ImGui::TreePop();
	}

	if (ImGui::BeginPopupContextItem()) {
		ImGui::Text(obj.GetName().c_str());
		if (ImGui::MenuItem("Delete")) {
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