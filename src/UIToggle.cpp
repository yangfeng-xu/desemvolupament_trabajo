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
		// 2. 【关键修复】如果正在冷却，直接跳过输入检测，只负责绘制
		if (cooldown > 0.0f) {
			// 依然需要绘制逻辑，不能直接 return
			// 复制下方的绘制代码到这里，或者只让输入检测受冷却限制
		}
		else {
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