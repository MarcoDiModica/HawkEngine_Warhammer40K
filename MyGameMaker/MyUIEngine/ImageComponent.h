#ifndef __IMAGE_COMPONENT_H__
#define __IMAGE_COMPONENT_H__
#pragma once

#include "UIComponent.h"
#include "MyGameEngine/Image.h"
#include "CanvasComponent.h"
#include "MyGameEngine/GameObject.h"
#include <memory>

class ImageComponent : public UIComponent {
public:
    ImageComponent(GameObject* owner);
    ~ImageComponent() override;

    void Start() const override;
    void Update(float deltaTime) const override;
    void Destroy() const override;

    CanvasComponent::UIComponentType GetType() const override { return CanvasComponent::UIComponentType::IMAGE; }

    void SetImage(std::shared_ptr<Image> image);
    std::shared_ptr<Image> GetImage() const;

    void Render() const;

private:
    std::shared_ptr<Image> image;
};

#endif // !__IMAGE_COMPONENT_H__
