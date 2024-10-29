#include "UIMainMenuBar.h"
#include "App.h"
#include "MyGUI.h"
#include <imgui.h>

#include "Log.h"
//libraries to open websites
#include <windows.h>
#include <shellapi.h>

UIMainMenuBar::UIMainMenuBar(UIType type, std::string name) : UIElement(type, name)
{
}

UIMainMenuBar::~UIMainMenuBar()
{
}

bool UIMainMenuBar::Draw()
{
	
	if (ImGui::BeginMainMenuBar()) {
		// Inicia el menú "General"


		if (ImGui::BeginMenu("General"))
		{

			if (ImGui::MenuItem("About")) { ShellExecute(0, 0, L"https://github.com/CITM-UPC/HawkEngine", 0, 0, SW_SHOW); }
			if (ImGui::MenuItem("Quit")) { /*codigo para salir de la app*/ }
			//Aqui abajo mas botones para esconder las diferentes ventanas de Imgui
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObjects"))
		{

			if (ImGui::MenuItem("Cube")) {}
			if (ImGui::MenuItem("Sphere")) {}
			ImGui::EndMenu();
		}
		// Finaliza la barra de menú principal
		ImGui::EndMainMenuBar();
	}
	
	return true;
}