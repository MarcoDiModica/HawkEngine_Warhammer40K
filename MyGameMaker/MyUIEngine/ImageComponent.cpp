#include "ImageComponent.h"

ImageComponent::ImageComponent(GameObject* owner) : Component(owner) {}

ImageComponent::~ImageComponent() {}

void ImageComponent::Start() {
    // Start implementation
}

void ImageComponent::Update(float deltaTime) {
    // Update implementation
}

void ImageComponent::Destroy() {
    // Destroy implementation
}

void ImageComponent::SetImage(std::shared_ptr<Image> image) {
    this->image = image;
}

std::shared_ptr<Image> ImageComponent::GetImage() const {
    return image;
}

bool ImageComponent::isInCanvas() const
{
	//check if the parent of its owner has a canvas component
	return owner->GetParent()->HasComponent<CanvasComponent>();
}


void ImageComponent::Render() const {
	//if the image is in a canvas, render the image
	if (isInCanvas()) {
		//render the image

	}
}
std::unique_ptr<Component> ImageComponent::Clone(GameObject* owner) {
	auto clone = std::make_unique<ImageComponent>(owner);
	clone->SetImage(image);
	return clone;
}
