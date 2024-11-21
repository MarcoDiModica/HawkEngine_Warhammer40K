#include "UIInspector.h"
#include "App.h"
#include "MyGUI.h"

#include "..\MyGameEngine\TransformComponent.h"
#include "..\MyGameEngine\Mesh.h"
#include "Camera.h"
#include "Input.h"
#include "..\MyGameEngine\types.h"
#include "..\MyGameEngine\MeshRendererComponent.h"
#include "..\MyGameEngine\Mesh.h"
#include "..\MyGameEngine\Image.h"
#include <string>

#include <imgui.h>
#include <imgui_internal.h>

UIInspector::UIInspector(UIType type, std::string name) : UIElement(type, name)
{
	matrixDirty = false;

	position = { 0, 0 ,0 };
	rotationRadians = { 0, 0, 0 };
	scale = { 1, 1, 1 };

}

UIInspector::~UIInspector()
{
}

bool UIInspector::Draw()
{
	ImGuiWindowFlags inspectorFlags = ImGuiWindowFlags_None | ImGuiWindowFlags_NoCollapse;
	ImGuiWindowClass windowClass;

	if (firstDraw)
	{
		ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
		firstDraw = false;
	}

	ImGui::SetNextWindowClass(&windowClass);
	windowClass.DockingAllowUnclassed = false;

	if (ImGui::Begin("Inspector", &enabled, inspectorFlags))
	{
		ImGuiIO& io = ImGui::GetIO();

		auto selectedGameObject = Application->input->GetSelectedGameObject();

		if (Application->input->GetSelectedGameObject() == nullptr)
		{
			ImGui::Text("No GameObject selected");
		}

		if (selectedGameObject != nullptr)
		{
			char newName[128] = {};
			strncpy_s(newName, selectedGameObject->GetName().c_str(), sizeof(newName));
			ImGui::SameLine(); ImGui::Text("GameObject:");
			if (ImGui::InputText("##GameObjectName", newName, sizeof(newName), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (strlen(newName) > 0)
				{
					selectedGameObject->SetName(newName);
				}
			}
			ImGui::SameLine(); ImGui::Checkbox("Static", &Application->input->GetSelectedGameObject()->isStatic);

			std::shared_ptr<Transform_Component> transform = selectedGameObject->GetTransform();

			if (transform)
			{
				
				if (ImGui::CollapsingHeader("Transform"))
				{
					//ImGui::Separator();

					glm::dvec3 currentPosition = transform->GetPosition();
					glm::dvec3 currentRotation = glm::radians(transform->GetEulerAngles());
					glm::dvec3 currentScale = transform->GetScale();

					float pos[3] = { static_cast<float>(currentPosition.x), static_cast<float>(currentPosition.y), static_cast<float>(currentPosition.z) };
					// Ojo que hay que castear a grados creo
					float rot[3] = { static_cast<float>(glm::degrees(currentRotation.x)), static_cast<float>(glm::degrees(currentRotation.y)), static_cast<float>(glm::degrees(currentRotation.z)) };
					float sca[3] = { static_cast<float>(currentScale.x), static_cast<float>(currentScale.y), static_cast<float>(currentScale.z) };

					if (ImGui::DragFloat3("Postition", pos, 0.1f))
					{
						glm::dvec3 newPosition = { pos[0], pos[1], pos[2] };
						glm::dvec3 deltaPos = newPosition - currentPosition;
						transform->Translate(deltaPos);
					}

					if (ImGui::DragFloat3("Rotation", rot, 0.1f))
					{
						glm::dvec3 newRotation = glm::radians(glm::dvec3(rot[0], rot[1], rot[2]));
						glm::dvec3 deltaRot = newRotation - currentRotation;

						//float newX = deltaRot.x * DEGTORAD;
						//float newY = deltaRot.y * DEGTORAD;
						//float newZ = deltaRot.z * DEGTORAD;

						transform->Rotate(deltaRot.x, glm::dvec3(1, 0, 0));
						transform->Rotate(deltaRot.y, glm::dvec3(0, 1, 0));
						transform->Rotate(deltaRot.z, glm::dvec3(0, 0, 1));
					}

					if (ImGui::DragFloat3("Scale", sca, 0.1f, 0.1f, 0.1f))
					{
						transform->Scale(glm::dvec3(sca[0], sca[1], sca[2]));
					}
					ImGui::Checkbox("Snap", &snap);
					ImGui::DragFloat("Snap Value", &snapValue, 0.1f, 0.1f,10.0f);

					
				}
			}

			ImGui::Separator();

			std::shared_ptr<Mesh> mesh = selectedGameObject->GetComponent<MeshRenderer>()->GetMesh();

			if (mesh)
			{
				ImGui::Text("Mesh");
				ImGui::Separator();

				ImGui::Text("Vertices: %d", mesh->vertices().size());
				ImGui::Text("Indices: %d", mesh->indices().size());
			
				bool& triNormals = mesh->drawTriangleNormals;
				bool& vertexNormals = mesh->drawVertexNormals;
				// bool& quadNormals = mesh->drawFaceNormals;

				ImGui::Checkbox("Tri Normals", &triNormals);
				ImGui::Checkbox("Vertex Normals", &vertexNormals);

			}

			ImGui::Separator();

			std::shared_ptr<Image> image = selectedGameObject->GetComponent<MeshRenderer>()->GetImage();
			if (image)
			{
				ImGui::Text("Image");
				ImGui::Separator();

				ImGui::Text("Width: %d", image->width());
				ImGui::Text("Heigth: %d", image->width());

				//ImGui::Checkbox("Checker Texture", image->LoadCheckerTexture())
			}
		}

		ImGui::End();
	}

	return true;
}