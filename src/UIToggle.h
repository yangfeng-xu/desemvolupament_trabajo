
#pragma once
#include "UIElement.h"

class UIToggle : public UIElement
{
public:
	// 这一行必须包含 Module* observer
	UIToggle(int id, SDL_Rect bounds, const char* text, bool initialBufferState, Module* observer);
	virtual ~UIToggle();

	bool Update(float dt) override;

	bool IsOn() const { return isOn; }

	void SetTextures(SDL_Texture* textureOff, SDL_Texture* textureOn);

private:
	bool isOn;
	bool canClick = true;
	float cooldown = 0.0f;
	SDL_Texture* texOff = nullptr;
	SDL_Texture* texOn = nullptr;
};