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

		static char filterBuffer[128] = "";
		ImGui::Text("Filter:");
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputText("##LogFilter", filterBuffer, IM_ARRAYSIZE(filterBuffer));
		ImGui::PopItemWidth();

		ImGui::Separator();

		// Log content
		const float PADDING = 8.0f; 
		const float MARGIN = 4.0f;  
		const float ROUNDING = 5.0f;

		ImGui::BeginChild("ScrollingArea", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		const auto& logs = Application->GetLogs();
		std::string filterText = filterBuffer;
		float contentWidth = ImGui::GetContentRegionAvail().x - MARGIN * 2;
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		for (int i = 0; i < logs.size(); i++)
		{
			const auto& log = logs[i];

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

			ImVec4 bgColor;
			ImVec4 typeColor;
			const char* typeText = "";
			switch (log.type)
			{
			case LogType::LOG_INFO:
				bgColor = ImVec4(0.05f, 0.2f, 0.05f, 0.6f);
				typeColor = ImVec4(0.0f, 1.0f, 0.2f, 1.0f);
				typeText = "[INFO]";
				break;
			case LogType::LOG_WARNING:
				bgColor = ImVec4(0.3f, 0.25f, 0.05f, 0.6f);
				typeColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
				typeText = "[WARNING]";
				break;
			case LogType::LOG_ASSIMP:
				bgColor = ImVec4(0.05f, 0.2f, 0.2f, 0.6f);
				typeColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
				typeText = "[ASSIMP]";
				break;
			case LogType::LOG_ERROR:
				bgColor = ImVec4(0.3f, 0.05f, 0.05f, 0.6f);
				typeColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
				typeText = "[ERROR]";
				break;
			case LogType::LOG_OK:
				bgColor = ImVec4(0.05f, 0.2f, 0.05f, 0.6f);
				typeColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
				typeText = "[OK]";
				break;
			case LogType::LOG_C_SHARP:
				bgColor = ImVec4(0.05f, 0.15f, 0.15f, 0.6f);
				typeColor = ImVec4(0.0f, 149.0f / 255.0f, 110.0f / 255.0f, 1.0f);
				typeText = "[C#]";
				break;
			case LogType::LOG_C_SHARP_WARNING:
				bgColor = ImVec4(0.3f, 0.25f, 0.05f, 0.6f);
				typeColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
				typeText = "[C# WARNING]";
				break;
			case LogType::LOG_C_SHARP_ERROR:
				bgColor = ImVec4(0.3f, 0.05f, 0.05f, 0.6f);
				typeColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
				typeText = "[C# ERROR]";
				break;
			}

			ImGui::PushID(i);

			ImGui::Dummy(ImVec2(0, MARGIN));

			float typeTextWidth = ImGui::CalcTextSize(typeText).x;
			float availableWidth = contentWidth - typeTextWidth - PADDING * 2 - ImGui::GetStyle().ItemSpacing.x;

			ImVec2 textSize = ImGui::CalcTextSize(log.message.c_str(), nullptr, false, availableWidth);

			float textHeight = textSize.y;

			float containerHeight = textHeight + PADDING * 2;

			ImVec2 containerStart = ImVec2(
				ImGui::GetCursorScreenPos().x + MARGIN,
				ImGui::GetCursorScreenPos().y
			);

			ImVec2 containerEnd = ImVec2(
				containerStart.x + contentWidth,
				containerStart.y + containerHeight
			);

			drawList->AddRectFilled(
				containerStart,
				containerEnd,
				ImGui::ColorConvertFloat4ToU32(bgColor),
				ROUNDING
			);

			drawList->AddRect(
				containerStart,
				containerEnd,
				ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.3f, 0.3f, 0.7f)),
				ROUNDING,
				0,
				1.0f
			);

			ImGui::SetCursorScreenPos(ImVec2(containerStart.x + PADDING, containerStart.y + PADDING));

			ImGui::TextColored(typeColor, "%s", typeText);

			ImGui::SameLine();
			ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + availableWidth);
			ImGui::Text("%s", log.message.c_str());
			ImGui::PopTextWrapPos();

			if (log.repeatCount > 1)
			{
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "(x%d)", log.repeatCount);
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
			{
				ImGui::OpenPopup("LogContextMenu");
			}

			if (ImGui::BeginPopup("LogContextMenu"))
			{
				if (ImGui::MenuItem("Copy Message"))
				{
					ImGui::SetClipboardText(log.message.c_str());
				}
				ImGui::EndPopup();
			}

			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, containerEnd.y));

			ImGui::PopID();
		}

		ImGui::Dummy(ImVec2(0, MARGIN));

		if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 20)
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
	}
	ImGui::End();
	return true;
}