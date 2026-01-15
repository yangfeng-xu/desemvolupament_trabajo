//#include "UIToggle.h"
//#include "Render.h"
//#include "Engine.h"
//#include "Input.h"
//
//UIToggle::UIToggle(int id, SDL_Rect bounds, const char* text, bool initialBufferState, Module* observer)
//    : UIElement(UIElementType::TOGGLE, id,bounds,text,observer), isOn(initialBufferState)
//{
//    this->bounds = bounds;
//    this->text = text;
//    this -> observer = observer;
//}
//
//UIToggle::~UIToggle() {}
//
//// 【新增】实现设置纹理
//void UIToggle::SetTextures(SDL_Texture* textureOff, SDL_Texture* textureOn) {
//    this->texOff = textureOff;
//    this->texOn = textureOn;
//}
//
//bool UIToggle::Update(float dt)
//{
//    if (state != UIElementState::DISABLED)
//    {
//        Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();
//
//        // ?????????
//        if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w &&
//            mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h)
//        {
//            state = UIElementState::FOCUSED;
//
//            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
//                state = UIElementState::PRESSED;
//            }
//
//            // ????????
//            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
//             
//                isOn = !isOn; // ????
//                NotifyObserver(); // ?? Scene
//            }
//        }
//        else {
//            state = UIElementState::NORMAL;
//        }
//
//        // 【修改】绘制逻辑
//         // 如果设置了纹理，就画纹理；否则还是画矩形（保持兼容性）
//        if (texOff != nullptr && texOn != nullptr) {
//            SDL_Texture* currentTex = isOn ? texOn : texOff;
//            // speed = 0.0f 确保 UI 不随地图滚动
//            Engine::GetInstance().render->DrawTexture(currentTex, bounds.x, bounds.y, nullptr, 0.0f);
//        }
//        else {
//            // 原来的方块逻辑
//            Uint8 r = isOn ? 0 : 255;
//            Uint8 g = isOn ? 255 : 0;
//            Engine::GetInstance().render->DrawRectangle(bounds, r, g, 0, 255, true, false);
//            Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h, { 255, 255, 255, 255 });
//        }
//        
//    }
//    return true;
//}
#include "UIToggle.h"
#include "Render.h"
#include "Engine.h"
#include "Input.h"

// 构造函数实现
UIToggle::UIToggle(int id, SDL_Rect bounds, const char* text, bool initialBufferState, Module* observer)
	: UIElement(UIElementType::TOGGLE, id, bounds, text, observer), isOn(initialBufferState)
{
	this->bounds = bounds;
	this->text = text;
}

UIToggle::~UIToggle() {}

void UIToggle::SetTextures(SDL_Texture* textureOff, SDL_Texture* textureOn) {
	this->texOff = textureOff;
	this->texOn = textureOn;
}

bool UIToggle::Update(float dt)
{
	if (cooldown > 0.0f) {
		cooldown -= dt;
	}
	if (state != UIElementState::DISABLED)
	{
		Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

		if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w &&
			mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h)
		{
			state = UIElementState::FOCUSED;

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				state = UIElementState::PRESSED;
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				isOn = !isOn;
				cooldown = 200.0f;
				NotifyObserver(); // 这里会调用 Scene::OnUIMouseClickEvent
			}
		}
		else {
			state = UIElementState::NORMAL;
		}

		// 绘制
		if (texOff != nullptr && texOn != nullptr) {
			SDL_Texture* currentTex = isOn ? texOn : texOff;
			Engine::GetInstance().render->DrawTexture(currentTex, bounds.x, bounds.y, nullptr, 0.0f);
		}
		else {
			Uint8 r = isOn ? 0 : 255;
			Uint8 g = isOn ? 255 : 0;
			Engine::GetInstance().render->DrawRectangle(bounds, r, g, 0, 255, true, false);
			Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h, { 255, 255, 255, 255 });
		}
	}
	return true;
}