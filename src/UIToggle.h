#pragma once
#include "UIElement.h"

class UIToggle : public UIElement
{
public:
    UIToggle(int id, SDL_Rect bounds, const char* text, bool initialBufferState);
    virtual ~UIToggle();

    bool Update(float dt) override;

    // ????????
    bool IsOn() const { return isOn; }

private:
    bool isOn; // true = ??, false = ??
    bool canClick = true;
};
