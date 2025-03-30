#include <imgui.h>

#include "UIConsole.h"
#include "App.h"
#include "MyGUI.h"
#include "Log.h"

UIConsole::UIConsole(UIType type, std::string name) : UIElement(type, name)
{
	showInfo = true;
	showWarnings = true;
	showErrors = true;
	showScriptOutput = true;
	showScriptOutput = true;
	autoScroll = true;
}

UIConsole::~UIConsole()
{
}

bool UIConsole::Draw()
{
	ImGuiWindowFlags consoleFlags = ImGuiWindowFlags_None;

	if (firstDraw)
	{
		ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_FirstUseEver);
		firstDraw = false;
	}

	if (ImGui::Begin("Console", &enabled, consoleFlags))
	{
		if (ImGui::SmallButton("Clear"))
		{
			Application->CleanLogs();
		}

		ImGui::SameLine();
		ImGui::Checkbox("Auto-scroll", &autoScroll);

		ImGui::SameLine();
		ImGui::Checkbox("Info", &showInfo);
		ImGui::SameLine();
		ImGui::Checkbox("Warnings", &showWarnings);
		ImGui::SameLine();
		ImGui::Checkbox("Errors", &showErrors);
		ImGui::SameLine();
		ImGui::Checkbox("Script", &showScriptOutput);

		ImGui::Separator();

		// Filter input
		static char filterBuffer[128] = "";
		ImGui::Text("Filter:");
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputText("##LogFilter", filterBuffer, IM_ARRAYSIZE(filterBuffer));
		ImGui::PopItemWidth();

		ImGui::Separator();

		// Log content
		ImGui::BeginChild("ScrollingArea", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);

		const auto& logs = Application->GetLogs();
		std::string filterText = filterBuffer;

		for (const auto& log : logs)
		{
			bool shouldShow = false;

			switch (log.type)
			{
			case LogType::LOG_INFO:
			case LogType::LOG_ASSIMP:
			case LogType::LOG_AUDIO:
			case LogType::LOG_OK:
				shouldShow = showInfo;
				break;
			case LogType::LOG_WARNING:
			case LogType::LOG_C_SHARP_WARNING:
				shouldShow = showWarnings;
				break;
			case LogType::LOG_ERROR:
			case LogType::LOG_C_SHARP_ERROR:
				shouldShow = showErrors;
				break;
			case LogType::LOG_C_SHARP:
				shouldShow = showScriptOutput;
				break;
			}

			if (!filterText.empty() && log.message.find(filterText) == std::string::npos)
			{
				shouldShow = false;
			}

			if (!shouldShow)
				continue;

			std::string logType;

			switch (log.type)
			{
			case LogType::LOG_INFO:
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.2f, 1.0f), "[INFO]");
				break;
			case LogType::LOG_WARNING:
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[WARNING]");
				break;
			case LogType::LOG_ASSIMP:
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "[ASSIMP]");
				break;
			case LogType::LOG_ERROR:
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "[ERROR]");
				break;
			case LogType::LOG_OK:
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[OK]");
				break;
			case LogType::LOG_C_SHARP:
				ImGui::TextColored(ImVec4(0.0f, 149.0f / 255.0f, 110.0f / 255.0f, 1.0f), "[C#]");
				break;
			case LogType::LOG_C_SHARP_WARNING:
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[C# WARNING]");
				break;
			case LogType::LOG_C_SHARP_ERROR:
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "[C# ERROR]");
				break;
			}

			ImGui::SameLine();
			ImGui::TextWrapped("%s", log.message.c_str());

			if (log.repeatCount > 1)
			{
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), " (%d)", log.repeatCount);
			}
		}

		if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10)
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
	}
	ImGui::End();
	return true;
}