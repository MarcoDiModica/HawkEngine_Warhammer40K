#include "UICamera.h"
#include "MyGUI.h"
#include "Input.h"

#include "imgui.h"

UICamera::UICamera(UIType type, std::string name) : UIElement(type, name)
{
}

UICamera::~UICamera()
{
}

bool UICamera::Draw()
{
	if (ImGui::Begin("Camera", &enabled))
	{
		// MalcomX
		ImGui::End();
	}

	return true;

}