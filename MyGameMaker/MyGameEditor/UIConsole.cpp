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
    ImGuiWindowFlags consoleFlags = ImGuiWindowFlags_None;

    if (firstDraw)
    {
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
		firstDraw = false; 
    }

    if (ImGui::Begin("Console", &enabled, consoleFlags))
    {
        if (ImGui::SmallButton("Clean"))
        {
            Application->CleanLogs();
        }

        ImGui::Separator();

        ImGui::BeginChild("Scrolling Area", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);

        for (const auto& log : Application->GetLogs())
        {
            std::string logType;

            switch (log.type)
            {
            case LogType::LOG_INFO:
                logType = "INFO";
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.2f, 1.0f), "INFO");
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
            case LogType::LOG_C_SHARP:
                logType = "C#";
                ImGui::TextColored(ImVec4(0.0f, 149.0f/255.0f , 110.0f/255.0f, 1.0f), "C#");
                break;
            }

            if (log.message[0] == '-')
                logType.insert(0, "\t");

            ImGui::SameLine();
            ImGui::TextUnformatted(log.message.c_str());
        }

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild(); 

        
    }
    ImGui::End();
	return true;
}