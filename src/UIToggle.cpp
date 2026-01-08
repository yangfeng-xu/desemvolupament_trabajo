#include "UIToggle.h"
#include "Render.h"
#include "Engine.h"
#include "Input.h"

UIToggle::UIToggle(int id, SDL_Rect bounds, const char* text, bool initialBufferState)
    : UIElement(UIElementType::TOGGLE, id), isOn(initialBufferState)
{
    this->bounds = bounds;
    this->text = text;
}

UIToggle::~UIToggle() {}

bool UIToggle::Update(float dt)
{
    if (state != UIElementState::DISABLED)
    {
        Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

        // ?????????
        if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w &&
            mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h)
        {
            state = UIElementState::FOCUSED;

            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
                state = UIElementState::PRESSED;
            }

            // ????????
            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
                isOn = !isOn; // ????
                NotifyObserver(); // ?? Scene
            }
        }
        else {
            state = UIElementState::NORMAL;
        }

        // ???? (?????????????????)
        // ?????????? Toggle ???
        Uint8 r = isOn ? 0 : 255;
        Uint8 g = isOn ? 255 : 0;

        Engine::GetInstance().render->DrawRectangle(bounds, r, g, 0, 255, true, false);
        Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h, { 255, 255, 255, 255 });
    }
    return true;
}