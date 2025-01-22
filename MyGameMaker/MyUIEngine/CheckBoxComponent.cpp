//#include "CheckBoxComponent.h"
//#include "GameObject.h"
//#include <algorithm>
//
//CheckBoxComponent::CheckBoxComponent(GameObject* owner) : Component(owner) {
//    // Initialization code here
//}
//
//CheckBoxComponent::~CheckBoxComponent() {
//    // Cleanup code here
//}
//
//void CheckBoxComponent::Start() {
//    // Start code here
//}
//
//void CheckBoxComponent::Update(float deltaTime) {
//    for (const auto& checkBox : checkBoxes) {
//        checkBox->Update(deltaTime);
//    }
//}
//
//void CheckBoxComponent::Destroy() {
//    // Destroy code here
//}
//
//void CheckBoxComponent::AddCheckBox(std::shared_ptr<UIElement> checkBox) {
//    checkBoxes.push_back(checkBox);
//}
//
//void CheckBoxComponent::RemoveCheckBox(std::shared_ptr<UIElement> checkBox) {
//    checkBoxes.erase(std::remove(checkBoxes.begin(), checkBoxes.end(), checkBox), checkBoxes.end());
//}
//
//void CheckBoxComponent::Render() {
//    for (const auto& checkBox : checkBoxes) {
//        if (checkBox->IsEnabled()) {
//            checkBox->Draw();
//        }
//    }
//}
//
//void CheckBoxComponent::SetOnCheckedChanged(std::function<void(bool)> onCheckedChanged) {
//    this->onCheckedChanged = onCheckedChanged;
//}