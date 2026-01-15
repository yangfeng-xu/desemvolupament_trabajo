//#pragma once
//
//#include "Input.h"
//#include "Render.h"
//#include "Module.h"
//#include "Vector2D.h"
//
//enum class UIElementType
//{
//	BUTTON,
//	TOGGLE,
//	CHECKBOX,
//	SLIDER,
//	SLIDERBAR,
//	COMBOBOX,
//	DROPDOWNBOX,
//	INPUTBOX,
//	VALUEBOX,
//	SPINNER
//};
//
//enum class UIElementState
//{
//	DISABLED,
//	NORMAL,
//	FOCUSED,
//	PRESSED,
//	SELECTED
//};
//
//class UIElement : public std::enable_shared_from_this<UIElement>
//{
//public:
//
//	UIElement() {}
//
//	// Constructor
//	UIElement(UIElementType type, int id) : type(type), id(id), state(UIElementState::NORMAL) {}
//
//	// Constructor
//	UIElement(UIElementType type, SDL_Rect bounds, const char* text) :
//		type(type),
//		state(UIElementState::NORMAL),
//		bounds(bounds),
//		text(text)
//	{
//		color.r = 255; color.g = 255; color.b = 255;
//		texture = NULL;
//	}
//
//	// Called each loop iteration
//	virtual bool Update(float dt)
//	{
//		return true;
//	}
//
//	// 
//	void SetTexture(SDL_Texture* tex)
//	{
//		texture = tex;
//		section = { 0, 0, 0, 0 };
//	}
//
//	// 
//	void SetObserver(Module* module)
//	{
//		observer = module;
//	}
//
//	// 
//	void NotifyObserver()
//	{
//		observer->OnUIMouseClickEvent(this);
//	}
//
//	virtual bool CleanUp()
//	{
//		return true;
//	}
//
//	virtual bool Destroy()
//	{
//		return true;
//	}
//
//public:
//
//	int id;
//	UIElementType type;
//	UIElementState state;
//
//	std::string text;       // UIElement text (if required)
//	SDL_Rect bounds;        // Position and size
//	SDL_Color color;        // Tint color
//
//	SDL_Texture* texture;   // Texture atlas reference
//	SDL_Rect section;       // Texture atlas base section
//
//	Module* observer;        // Observer 
//};
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

	// 统一的构造函数
	UIElement(UIElementType type, int id, SDL_Rect bounds, const char* text, Module* observer) :
		type(type),
		id(id),
		state(UIElementState::NORMAL),
		bounds(bounds),
		text(text),
		observer(observer) // 关键：保存 observer
	{
		color.r = 255; color.g = 255; color.b = 255; color.a = 255;
		texture = nullptr;
		section = { 0, 0, 0, 0 };
	}

	virtual ~UIElement() {}

	virtual bool Update(float dt) { return true; }

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

	std::string text;
	SDL_Rect bounds;
	SDL_Color color;

	SDL_Texture* texture = nullptr;
	SDL_Rect section;

	Module* observer = nullptr; // 默认为空
};