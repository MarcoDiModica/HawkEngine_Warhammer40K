//#include "SceneManagement.h"
//
//SceneManagement::SceneManagement() {}
//
//void SceneManagement::CreateScene(const std::string& name) {
//    auto scene = std::make_shared<Scene>(name);
//    scenes.push_back(scene);
//    if (!activeScene) {
//        activeScene = scene;
//    }
//}
//
//void SceneManagement::AddScene(std::shared_ptr<Scene> scene) {
//    scenes.push_back(scene);
//    if (!activeScene) {
//        activeScene = scene;
//    }
//}
//
//void SceneManagement::RemoveScene(const std::string& name) {
//    for (auto it = scenes.begin(); it != scenes.end(); ++it) {
//        if ((*it)->GetName() == name) {
//            if (*it == activeScene) {
//                activeScene = nullptr;
//            }
//            scenes.erase(it);
//            return;
//        }
//    }
//}
//
//void SceneManagement::SetActiveScene(const std::string& name) {
//    for (const auto& scene : scenes) {
//        if (scene->GetName() == name) {
//            activeScene = scene;
//            return;
//        }
//    }
//}
//
//std::shared_ptr<Scene> SceneManagement::GetActiveScene() const {
//    return activeScene;
//}
//
//void SceneManagement::Start() {
//	if (activeScene) {
//		activeScene->Start();
//	}
//}
//
//void SceneManagement::Update(float deltaTime) {
//	if (activeScene) {
//		activeScene->Update(deltaTime);
//	}
//}
//
//void SceneManagement::Destroy() {
//	for (auto& scene : scenes) {
//		scene->Destroy();
//	}
//	scenes.clear();
//	activeScene = nullptr;
//}