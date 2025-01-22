#include "UIImage.h"
#include <iostream> 

UIImage::UIImage(const std::string& imagePath) : UIElement(UIType::UNKNOWN, "UIImage"), imagePath(imagePath) {

}

UIImage::~UIImage() {

}

void UIImage::Update(float deltaTime) {

}

bool UIImage::Draw() {

    std::cout << "Drawing image from path: " << imagePath << std::endl;
    return true;
}
