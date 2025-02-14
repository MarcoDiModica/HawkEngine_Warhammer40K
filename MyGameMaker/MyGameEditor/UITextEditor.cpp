#include "UITextEditor.h"
#include <imgui.h>
#include <algorithm>

UITextEditor::UITextEditor(UIType type, std::string name) : UIElement(type, name) {
    buffer.resize(1024, 0);
    modified = false;
}

bool UITextEditor::Draw() {
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

    if (firstDraw)
    {
        ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
		firstDraw = false;
    }

    if (!ImGui::Begin("Text Editor", &enabled, windowFlags)) {
		ImGui::End();
		return false;
	}

    ImGui::InputTextMultiline("##TextEditor", buffer.data(), buffer.size(), ImVec2(-1, -1), ImGuiInputTextFlags_AllowTabInput);

    if (ImGui::Button("Save")) {
		modified = true;
	}

    ImGui::End();

    return true;
}

void UITextEditor::SetText(const std::string& text) {
    if (text.size() + 1 > buffer.size()) {
        buffer.resize(text.size() + 1);
    }
    std::copy(text.begin(), text.end(), buffer.begin());
    buffer[text.size()] = '\0';
    modified = false;
}

std::string UITextEditor::GetText() const {
    return std::string(buffer.data());
}