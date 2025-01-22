#ifndef UICOMPONENT_H
#define UICOMPONENT_H

#include "CanvasComponent.h"
#include <string>
#include <functional>

class UIComponent {
public:
    UIComponent();
    UIComponent(const std::string& name);
    virtual ~UIComponent();

    std::string getName() const;
    void setName(const std::string& name);

    virtual void Start()const;
    virtual void Update(float deltaTime)const;
    virtual void Destroy()const;

    virtual CanvasComponent::UIComponentType GetType() const;

    virtual void render() const = 0;

    // M�todos espec�ficos para CheckBoxComponent
    virtual bool isChecked() const;
    virtual void setChecked(bool checked);
    virtual void toggle();

    // M�todos espec�ficos para ButtonComponent
    virtual void onClick() const;
    virtual void setOnClickHandler(const std::function<void()>& handler);

    // M�todos espec�ficos para InputBoxComponent
    virtual std::string getText() const;
    virtual void setText(const std::string& text);
    virtual void onTextChanged() const;
    virtual void setOnTextChangedHandler(const std::function<void(const std::string&)>& handler);


    // M�todos espec�ficos para ImageComponent
    virtual std::string getImagePath() const;
    virtual void setImagePath(const std::string& path);
    virtual void onImageChanged() const;
    virtual void setOnImageChangedHandler(const std::function<void(const std::string&)>& handler);

private:
    std::string name;
    bool checked; // A�adido para soportar el estado de CheckBox
    std::function<void()> onClickHandler; // A�adido para soportar el evento onClick
    std::string text; // A�adido para soportar el texto de InputBox
    std::function<void(const std::string&)> onTextChangedHandler; // A�adido para soportar el evento onTextChanged
    std::string imagePath; // A�adido para soportar la ruta de la imagen de ImageComponent
    std::function<void(const std::string&)> onImageChangedHandler; // A�adido para soportar el evento onImageChanged
};

#endif // UICOMPONENT_H