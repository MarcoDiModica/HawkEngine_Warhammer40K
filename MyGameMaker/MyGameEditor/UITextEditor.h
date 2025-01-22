#ifndef UI_TEXT_EDITOR_H
#define UI_TEXT_EDITOR_H

#include "UIElement.h"
#include <vector>
#include <string>

class UITextEditor : public UIElement
{
public:
    UITextEditor(UIType type, std::string name);

    bool Draw() override;

    void SetText(const std::string& text);
    std::string GetText() const;

private:
    std::vector<char> buffer;
    bool modified;
};

#endif //UI_TEXT_EDITOR_H