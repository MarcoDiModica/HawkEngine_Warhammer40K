#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <SDL2/SDL.h>

#include "UIHierarchy.h"
#include "App.h"
#include "Root.h"
#include "MyGameEditor/Input.h"
#include "MyGameEngine/GameObject.h"
#include "DragDropManager.h"

UIHierarchy::UIHierarchy(UIType type, std::string name) : UIElement(type, name) {
}

UIHierarchy::~UIHierarchy() {

}

bool UIHierarchy::Draw() {
	static bool showSavePopup = false;

	if (Application->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT &&
		Application->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) {
		showSavePopup = true;
	}

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

				bool enterPressed = ImGui::InputText("##RenameScene", currentSceneName.data(), 100, ImGuiInputTextFlags_EnterReturnsTrue);

				if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
					ImGui::SetKeyboardFocusHere(-1);

				if (enterPressed || (ImGui::IsItemDeactivated() && ImGui::IsMouseClicked(0))) {
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

		if (DragDropManager::draggedObject && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && DragDropManager::draggedObject->GetParent()) {
			GameObject* p = DragDropManager::draggedObject->GetParent();
			Transform_Component* dragTransform = DragDropManager::draggedObject->GetTransform();

			if (dragTransform) {
				glm::dmat4 worldMatrix = dragTransform->GetMatrix();

				currentScene->AddGameObject(DragDropManager::draggedObject->shared_from_this());
				if (p) {
					p->RemoveChild(DragDropManager::draggedObject);
				}
				dragTransform = DragDropManager::draggedObject->GetTransform();
				if (dragTransform) {
					dragTransform->SetMatrix(worldMatrix);
				}
			}

			DragDropManager::draggedObject = nullptr;
		}
	}

	if (showSavePopup) {
		ImGui::OpenPopup("Save Scene");
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Save Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Do you want to save the current scene?");
		ImGui::Separator();

		if (ImGui::Button("Save", ImVec2(120, 0))) {
			Application->scene_serializer->Serialize("Library/Scenes");
			showSavePopup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			showSavePopup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
	ImGui::PopStyleColor();
	return true;
}

void UIHierarchy::RenderSceneHierarchy(Scene* currentScene) {
	int size = static_cast<int>(Application->root->GetActiveScene()->children().size());

	for (auto& go : Application->root->GetActiveScene()->children()) {
		DrawSceneObject(*go);
	}
}

bool UIHierarchy::DrawSceneObject(GameObject& obj)
{
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

		bool hasPrefabPath = !obj.GetPrefabSourcePath().empty();
		if (hasPrefabPath) {
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
				ImVec4 textColor = obj.isStatic
					? ImVec4(0.7f, 0.7f, 0.7f, 1.0f)
					: ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, textColor);
				ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.15f, 0.15f, 0.15f, 0.0f));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.25f, 0.25f, 0.25f, 0.6f));
			}
		}		
	}

	bool open = ImGui::TreeNodeEx(obj.GetName().c_str(), nodeFlags);

	static struct {
		bool mouseDownOnThisItem = false;
		GameObject* item = nullptr;
		bool wasDragged = false;
	} clickState;

	if (ImGui::IsItemClicked(0) && !ImGui::IsItemToggledOpen()) {
		clickState.mouseDownOnThisItem = true;
		clickState.item = &obj;
		clickState.wasDragged = false;
	}

	ImGui::PopStyleColor(4);

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		if (clickState.item == &obj) {
			clickState.wasDragged = true;
		}

		ImGui::SetDragDropPayload("GAMEOBJECT", &obj, sizeof(GameObject*));
		ImGui::Text("Dragging %s, gid %d", obj.GetName().c_str(), obj.GetID());
		DragDropManager::draggedObject = &obj;
		ImGui::EndDragDropSource();
	}

	if (clickState.mouseDownOnThisItem && ImGui::IsMouseReleased(0)) {
		if (!clickState.wasDragged && clickState.item == &obj) {
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

		if (clickState.item == &obj) {
			clickState.mouseDownOnThisItem = false;
			clickState.item = nullptr;
			clickState.wasDragged = false;
		}
	}

	if (DragDropManager::draggedObject) {
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT")) {
				if (!DragDropManager::draggedObject) {
					DragDropManager::draggedObject = *(GameObject**)payload->Data;
				}
				Application->root->ParentGameObjectPreserve(*DragDropManager::draggedObject, obj);
				DragDropManager::draggedObject = nullptr;
				should_continue = false;
			}
			ImGui::EndDragDropTarget();
		}
	}

	if (ImGui::BeginPopupContextItem()) {
		ImGui::Text(obj.GetName().c_str());
		ImGui::Separator();
		if (ImGui::MenuItem("Delete")) {
			Application->input->RemoveFromSelection(&obj);
			Application->input->ClearSelection();
			Application->root->RemoveGameObject(&obj);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Empty Parent")) {
			//TODO LIKE UNITY
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Empty Child")) {
			auto empty = Application->root->CreateGameObject("Empty");
			Application->root->ParentGameObject(*empty, obj);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Rename")) {
			// TODO: Implement renaming like scene one
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Duplicate")) {
			//TODO: Implement duplication like ctrl D
			ImGui::CloseCurrentPopup();
		}
		if (hasChildren) {
			if (ImGui::MenuItem("Expand All")) {
				//TODO
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Collapse All")) {
				//TODO
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::EndPopup();
	}

	if (open) {
		for (size_t w = 0; w < obj.GetChildren().size(); ++w) {
			DrawSceneObject(*obj.GetChildren()[w]);
		}
		ImGui::TreePop();
	}

	return should_continue;
}