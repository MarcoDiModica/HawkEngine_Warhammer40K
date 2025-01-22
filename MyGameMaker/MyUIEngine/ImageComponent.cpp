#include "ImageComponent.h"
#include <GL/glew.h>

ImageComponent::ImageComponent(GameObject* owner) : UIComponent(owner) {
    // Initialization code here
}

ImageComponent::~ImageComponent() {
    // Cleanup code here
}

void ImageComponent::Start()const {
    // Start code here
}

void ImageComponent::Update(float deltaTime)const {
    // Update code here
}

void ImageComponent::Destroy()const {
    // Destroy code here
}

void ImageComponent::SetImage(std::shared_ptr<Image> image) {
    this->image = image;
}

std::shared_ptr<Image> ImageComponent::GetImage() const {
    return image;
}

void ImageComponent::Render() const {
    if (image) {
        image->bind();
        // Render the image using OpenGL or your preferred rendering method
        // For example, you can use a textured quad to display the image
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.5f, -0.5f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(0.5f, -0.5f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(0.5f, 0.5f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.5f, 0.5f);
        glEnd();
    }
}
