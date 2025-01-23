#ifndef __IMAGE_COMPONENT_H__
#define __IMAGE_COMPONENT_H__
#pragma once

#include "MyGameEngine/Component.h"
#include "MyGameEngine/Image.h"
#include "CanvasComponent.h"
#include "MyGameEngine/GameObject.h"
#include <memory>

class ImageComponent : public Component {
public:
    ImageComponent(GameObject* owner);
    ~ImageComponent() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;
    std::unique_ptr<Component> Clone(GameObject* owner) override;


    ComponentType GetType() const override { return ComponentType::IMAGE; }

    void SetImage(std::shared_ptr<Image> image);
    std::shared_ptr<Image> GetImage() const;

    bool isInCanvas() const;

    void Render() const;

private:
    std::shared_ptr<Image> image;
};

#endif // !__IMAGE_COMPONENT_H__
