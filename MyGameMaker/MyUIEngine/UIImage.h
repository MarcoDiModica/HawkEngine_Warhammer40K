#ifndef __UI_IMAGE_H__
#define __UI_IMAGE_H__
#pragma once

#include "MyGameEditor/UIElement.h"
#include <string>

class UIImage : public UIElement {
public:
    UIImage(const std::string& imagePath);
    ~UIImage() override;

    void Update(float deltaTime) override;
    bool Draw() override;

private:
    std::string imagePath;

};

#endif // !__UI_IMAGE_H__
