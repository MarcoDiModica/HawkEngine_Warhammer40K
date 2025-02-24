#include <imgui.h>

#include "UIGameView.h"
#include "MyGUI.h"
#include "MyWindow.h"
#include "App.h"
#include "MyGameEngine/CameraComponent.h"

UIGameView::UIGameView(UIType type, std::string name) : UIElement(type, name)
{
}

UIGameView::~UIGameView()
{
    if (Application->gui->fboGame != 0) {
        glDeleteFramebuffers(1, &Application->gui->fboGame);
        glDeleteRenderbuffers(1, &Application->gui->rboGame);
        glDeleteTextures(1, &Application->gui->fboTextureGame);
    }
}

void UIGameView::Init()
{
    glGenFramebuffers(1, &Application->gui->fboGame);
    glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fboGame);

    glGenTextures(1, &Application->gui->fboTextureGame);
    glBindTexture(GL_TEXTURE_2D, Application->gui->fboTextureGame);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Application->window->width(), Application->window->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application->gui->fboTextureGame, 0);

    glGenRenderbuffers(1, &Application->gui->rboGame);
    glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rboGame);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Application->window->width(), Application->window->height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Application->gui->rboGame);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG(LogType::LOG_ERROR, "Game View Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UIGameView::UpdateFramebuffer()
{
    int newWidth = Application->window->width();
    int newHeight = Application->window->height();

    glBindTexture(GL_TEXTURE_2D, Application->gui->fboTextureGame);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, Application->gui->rboGame);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newWidth, newHeight);
}

bool UIGameView::Draw()
{
    if (winSize.x != Application->window->width() || winSize.y != Application->window->height())
    {
        UpdateFramebuffer();
    }

    if (ImGui::Begin("Game View", &enabled))
    {
        if (Application->root->mainCamera == nullptr)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "No game camera found!");
            ImGui::TextWrapped("Create a GameObject with a CameraComponent and set it as main camera.");
            ImGui::End();
            return true;
        }

        ImGui::PushStyleColor(ImGuiCol_Button, isPlaying ? ImVec4(0.8f, 0.2f, 0.2f, 1.0f) : ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
        if (ImGui::Button(isPlaying ? "  STOP  " : "  PLAY  "))
        {
            isPlaying = !isPlaying;
        }
        ImGui::PopStyleColor();

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        winPos = vec2(windowPos.x, windowPos.y);
        winSize = vec2(windowSize.x, windowSize.y);

        ImVec2 availableSize = ImGui::GetContentRegionAvail();

        float aspectRatio = static_cast<float>(Application->window->width()) / Application->window->height();

        float width = availableSize.x;
        float height = width / aspectRatio;

        if (height > availableSize.y)
        {
            height = availableSize.y;
            width = height * aspectRatio;
        }

        float offsetX = (availableSize.x - width) * 0.5f;
        float offsetY = (availableSize.y - height) * 0.5f;

        ImGui::SetCursorPos(ImVec2(offsetX, offsetY));

        ImGui::Image((ImTextureID)(uintptr_t)Application->gui->fboTextureGame,
            ImVec2(width, height),
            ImVec2(0, 1), ImVec2(1, 0));

        if (isPlaying)
        {
            ImVec2 textSize = ImGui::CalcTextSize("PLAYING");
            ImGui::SetCursorPos(ImVec2(offsetX + width - textSize.x - 10, offsetY + 10));
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "PLAYING");
        }

        ImGui::End();
    }

    return true;
}