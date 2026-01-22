#pragma once

#include "Input.h"
#include "Render.h"
#include "Module.h"
#include "Vector2D.h"
#include <memory>
#include <string>

enum class UIElementType
{
	BUTTON,
	TOGGLE,
	CHECKBOX,
	SLIDER,
	SLIDERBAR,
	COMBOBOX,
	DROPDOWNBOX,
	INPUTBOX,
	VALUEBOX,
	SPINNER
};

enum class UIElementState
{
	DISABLED,
	NORMAL,
	FOCUSED,
	PRESSED,
	SELECTED
};

class UIElement : public std::enable_shared_from_this<UIElement>
{
public:
	UIElement() {}
	UIElement(UIElementType type, int id, SDL_Rect bounds, const char* text, Module* observer) :
		type(type),
		id(id),
		state(UIElementState::NORMAL),
		bounds(bounds),
		text(text),
		observer(observer) 
	{
		color.r = 255; color.g = 255; color.b = 255; color.a = 255;
		texture = nullptr;
		section = { 0, 0, 0, 0 };
	}

	virtual ~UIElement() {}

	virtual bool Update(float dt) { return true; }
	virtual void Render() {}
	void SetTexture(SDL_Texture* tex)
	{
		texture = tex;
		section = { 0, 0, 0, 0 };
	}

	void SetObserver(Module* module)
	{
		observer = module;
	}

	void NotifyObserver()
	{
		if (observer != nullptr) {
			observer->OnUIMouseClickEvent(this);
		}
	}

	virtual bool CleanUp() { return true; }
	virtual bool Destroy() { return true; }

public:
	int id;
	UIElementType type;
	UIElementState state;
	bool visible = true;

	std::string text;
	SDL_Rect bounds;
	SDL_Color color;

	SDL_Texture* texture = nullptr;
	SDL_Rect section;

	Module* observer = nullptr; 
};