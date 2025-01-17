#include "UIAudioTest.h"
#include "imgui.h"
#include <filesystem>

namespace MyGameEngine {

UIAudioTest::UIAudioTest()
    : m_CurrentMusicSource(0)
    , m_CurrentSFXSource(0)
    , m_ListenerPosition{0.0f, 0.0f, 0.0f}
    , m_SFXPosition{0.0f, 0.0f, 0.0f}
    , m_Volume(1.0f)
    , m_MasterVolume(1.0f)
    , m_ShowWindow(false)
{
    strcpy_s(m_MusicPath, "test_sounds/music.wav");
    strcpy_s(m_SFXPath, "test_sounds/effect.wav");
}

void UIAudioTest::Initialize() {
    m_AudioEngine = std::make_shared<AudioEngine>();
    if (!m_AudioEngine->Initialize()) {
        // Handle initialization failure
        m_AudioEngine.reset();
    }
}

void UIAudioTest::OnImGui() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::MenuItem("Audio Test")) {
                m_ShowWindow = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (!m_ShowWindow) return;

    ImGui::Begin("Audio Test Panel", &m_ShowWindow);

    if (!m_AudioEngine || !m_AudioEngine->IsInitialized()) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Audio Engine not initialized!");
        if (ImGui::Button("Initialize Audio Engine")) {
            Initialize();
        }
        ImGui::End();
        return;
    }

    if (ImGui::CollapsingHeader("Audio Device Info", ImGuiTreeNodeFlags_DefaultOpen)) {
        DrawAudioDeviceInfo();
    }

    if (ImGui::CollapsingHeader("Background Music", ImGuiTreeNodeFlags_DefaultOpen)) {
        DrawMusicControls();
    }

    if (ImGui::CollapsingHeader("Sound Effects", ImGuiTreeNodeFlags_DefaultOpen)) {
        DrawSoundEffectControls();
    }

    if (ImGui::CollapsingHeader("3D Audio Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        Draw3DAudioControls();
    }

    // Master volume control
    if (ImGui::SliderFloat("Master Volume", &m_MasterVolume, 0.0f, 1.0f)) {
        m_AudioEngine->SetMasterVolume(m_MasterVolume);
    }

    // Active sources info
    if (ImGui::Button("Print Active Sources Info")) {
        m_AudioEngine->PrintActiveSourcesInfo();
    }

    ImGui::End();
}

void UIAudioTest::DrawAudioDeviceInfo() {
    if (ImGui::Button("Refresh Device Info")) {
        m_AudioEngine->PrintAudioDeviceInfo();
    }
}

void UIAudioTest::DrawMusicControls() {
    ImGui::InputText("Music File Path", m_MusicPath, sizeof(m_MusicPath));
    
    ImGui::BeginGroup();
    if (ImGui::Button("Play Music")) {
        if (m_CurrentMusicSource != 0) {
            m_AudioEngine->StopSound(m_CurrentMusicSource);
        }
        m_CurrentMusicSource = m_AudioEngine->TestPlayMusic(m_MusicPath, true);
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop Music") && m_CurrentMusicSource != 0) {
        m_AudioEngine->StopSound(m_CurrentMusicSource);
        m_CurrentMusicSource = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause/Resume") && m_CurrentMusicSource != 0) {
        if (m_AudioEngine->IsPlaying(m_CurrentMusicSource)) {
            m_AudioEngine->PauseSound(m_CurrentMusicSource);
        } else {
            m_AudioEngine->ResumeSound(m_CurrentMusicSource);
        }
    }
    ImGui::EndGroup();

    if (m_CurrentMusicSource != 0) {
        if (ImGui::SliderFloat("Music Volume", &m_Volume, 0.0f, 1.0f)) {
            m_AudioEngine->SetVolume(m_CurrentMusicSource, m_Volume);
        }
    }
}

void UIAudioTest::DrawSoundEffectControls() {
    ImGui::InputText("SFX File Path", m_SFXPath, sizeof(m_SFXPath));
    
    ImGui::BeginGroup();
    if (ImGui::Button("Play Sound Effect")) {
        if (m_CurrentSFXSource != 0) {
            m_AudioEngine->StopSound(m_CurrentSFXSource);
        }
        m_CurrentSFXSource = m_AudioEngine->TestPlaySoundEffect(m_SFXPath, 
            m_SFXPosition[0], m_SFXPosition[1], m_SFXPosition[2]);
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop SFX") && m_CurrentSFXSource != 0) {
        m_AudioEngine->StopSound(m_CurrentSFXSource);
        m_CurrentSFXSource = 0;
    }
    ImGui::EndGroup();

    if (m_CurrentSFXSource != 0) {
        if (ImGui::SliderFloat("SFX Volume", &m_Volume, 0.0f, 1.0f)) {
            m_AudioEngine->SetVolume(m_CurrentSFXSource, m_Volume);
        }
    }
}

void UIAudioTest::Draw3DAudioControls() {
    // Listener position control
    if (ImGui::DragFloat3("Listener Position", m_ListenerPosition, 0.1f)) {
        m_AudioEngine->TestSetListenerPosition(
            m_ListenerPosition[0], m_ListenerPosition[1], m_ListenerPosition[2]);
    }

    // Sound effect position control
    if (ImGui::DragFloat3("SFX Position", m_SFXPosition, 0.1f)) {
        if (m_CurrentSFXSource != 0) {
            m_AudioEngine->SetSourcePosition(m_CurrentSFXSource,
                m_SFXPosition[0], m_SFXPosition[1], m_SFXPosition[2]);
        }
    }
}

} 