#include "UIConsole.h"
#include "App.h"
#include "MyGUI.h"
#include <imgui.h>

#include "Log.h"

UIConsole::UIConsole(UIType type, std::string name) : UIElement(type, name)
{
}

UIConsole::~UIConsole()
{
}

bool UIConsole::Draw()
{
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, 200), ImVec2(800, 600));

	ImGuiWindowFlags consoleFlags = 0;
	consoleFlags |= ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin("Console", &enabled, consoleFlags))
	{
		if (ImGui::SmallButton("Clean"))
		{
			Application->CleanLogs();
		}

		ImGui::Separator();

		ImGuiWindowFlags scrollFlags = 0;
		scrollFlags |= ImGuiWindowFlags_HorizontalScrollbar;
		scrollFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;

		if (ImGui::BeginChild("Scrolling", ImVec2(0, 0), false, scrollFlags))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 15));

			std::string logType;

			// Iterate through array of logs, switch between logtypes
			for (const auto& log : Application->GetLogs())
			{
				switch (log.type)
				{
				case LogType::LOG_INFO:
					logType = "INFO";
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "INFO");
					break;
				case LogType::LOG_WARNING:
					logType = "WARNING";
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "WARNING");
					break;
				case LogType::LOG_ASSIMP:
					logType = "ASSIMP";
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "ASSIMP");
					break;
				case LogType::LOG_ERROR:
					logType = "ERROR";
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "ERROR");
					break;
				case LogType::LOG_OK:
					logType = "OK";
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "OK");
					break;
				}
				if (log.message[0] == '-')
					logType.insert(0, "\t");

				ImGui::SameLine();
				ImGui::TextUnformatted(log.message.c_str());
				
			}
			
			ImGui::PopStyleVar();

			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);

			ImGui::EndChild();
		}

		ImGui::End();	

	}
	return true;
}