#include "UIAudioTest.h"
#include "App.h"
#include "MyGUI.h"
#include <imgui.h>

UIAudioTest::UIAudioTest(UIType type, std::string name) : UIElement(type, name)
{
    m_AudioEngine = std::make_shared<MyGameEngine::AudioEngine>();
    if (!m_AudioEngine->Initialize()) {
        LOG(LogType::LOG_ERROR, "Failed to initialize Audio Engine");
    } else {
        LOG(LogType::LOG_OK, "Audio Engine initialized successfully");
    }
}

UIAudioTest::~UIAudioTest()
{
    if (m_AudioEngine) {
        m_AudioEngine->Shutdown();
    }
}

bool UIAudioTest::Draw()
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

    if (firstDraw) {
        ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
        firstDraw = false;
    }

    if (!ImGui::Begin("Audio Test", &enabled, windowFlags)) {
        ImGui::End();
        return false;
    }

    // Device Information Section
    if (ImGui::CollapsingHeader("Audio Device Info", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Print Device Info")) {
            m_AudioEngine->PrintAudioDeviceInfo();
        }
        ImGui::SameLine();
        if (ImGui::Button("Print Active Sources")) {
            m_AudioEngine->PrintActiveSourcesInfo();
        }
    }

    // Background Music Section
    if (ImGui::CollapsingHeader("Background Music", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputText("Music File Path", m_MusicPath, sizeof(m_MusicPath));
        
        if (ImGui::Button("Play Music")) {
            m_CurrentMusicSource = m_AudioEngine->TestPlayMusic(m_MusicPath, true);
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop Music") && m_CurrentMusicSource != 0) {
            m_AudioEngine->StopSound(m_CurrentMusicSource);
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause/Resume") && m_CurrentMusicSource != 0) {
            if (m_AudioEngine->IsPlaying(m_CurrentMusicSource)) {
                m_AudioEngine->PauseSound(m_CurrentMusicSource);
            } else {
                m_AudioEngine->ResumeSound(m_CurrentMusicSource);
            }
        }
    }

    // Sound Effects Section
    if (ImGui::CollapsingHeader("Sound Effects", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputText("Effect File Path", m_SoundEffectPath, sizeof(m_SoundEffectPath));
        
        ImGui::Text("Effect Position");
        ImGui::DragFloat3("Position", m_Position, 0.1f);
        
        if (ImGui::Button("Play Effect")) {
            m_CurrentEffectSource = m_AudioEngine->TestPlaySoundEffect(
                m_SoundEffectPath, 
                m_Position[0], 
                m_Position[1], 
                m_Position[2]
            );
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop Effect") && m_CurrentEffectSource != 0) {
            m_AudioEngine->StopSound(m_CurrentEffectSource);
        }
    }

    // Listener Settings
    if (ImGui::CollapsingHeader("Listener Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Listener Position");
        if (ImGui::DragFloat3("Listener Pos", m_ListenerPosition, 0.1f)) {
            m_AudioEngine->TestSetListenerPosition(
                m_ListenerPosition[0],
                m_ListenerPosition[1],
                m_ListenerPosition[2]
            );
        }
    }

    // Volume Controls
    if (ImGui::CollapsingHeader("Volume Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::SliderFloat("Master Volume", &m_MasterVolume, 0.0f, 1.0f)) {
            m_AudioEngine->SetMasterVolume(m_MasterVolume);
        }

        ImGui::Text("Individual Source Volume");
        if (ImGui::SliderFloat("Source Volume", &m_Volume, 0.0f, 1.0f)) {
            if (m_CurrentMusicSource != 0) {
                m_AudioEngine->SetVolume(m_CurrentMusicSource, m_Volume);
            }
            if (m_CurrentEffectSource != 0) {
                m_AudioEngine->SetVolume(m_CurrentEffectSource, m_Volume);
            }
        }
    }

    // Format Testing
    if (ImGui::CollapsingHeader("Format Testing")) {
        if (ImGui::Button("Test All Formats")) {
            m_AudioEngine->TestPlayAllSupportedFormats();
        }
    }

    ImGui::End();
    return true;
} 