#include "UIManager.h"
#include "UIButton.h"
#include "UIToggle.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"

UIManager::UIManager() :Module()
{
	name = "UIManager";
}

UIManager::~UIManager() {}

bool UIManager::Start()
{
	return true;
}

std::shared_ptr<UIElement> UIManager::CreateUIElement(UIElementType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds)
{
	std::shared_ptr<UIElement> uiElement = std::make_shared<UIElement>();

	// L16: TODO 1: Implement CreateUIElement function that instantiates a new UIElement according to the UIElementType and add it to the list of UIElements
	switch(type) {
	case UIElementType::BUTTON:
		uiElement = std::make_shared<UIButton>(id, bounds, text);
		break;
	case UIElementType::TOGGLE:
		uiElement = std::make_shared<UIToggle>(id,bounds,text,false);
		break;
	case UIElementType::CHECKBOX:
		break;
	case UIElementType::SLIDER:
		break;
	case UIElementType::SLIDERBAR:
		break;
	case UIElementType::COMBOBOX:
		break;
	case UIElementType::DROPDOWNBOX:
		break;
	case UIElementType::INPUTBOX:
		break;
	case UIElementType::VALUEBOX:
		break;
	case UIElementType::SPINNER:
		break;
	}
	//set the callback to the observer
	uiElement->SetObserver(observer);
	//Add UiElemento to the list
	UIElementsList.push_back(uiElement);
	return uiElement;
}


std::shared_ptr<UIElement> UIManager::GetElement(int id)
{
	for (auto& uiElement : UIElementsList)
	{
		if (uiElement->id == id)
		{
			return uiElement;
		}
	}
	return nullptr; // ???????????
}

bool UIManager::Update(float dt)
{	
	for (const auto& control : UIElementsList)
	{
		control->Update(dt);
	}

	return true;
}

bool UIManager::CleanUp()
{
	for (const auto& uiElement : UIElementsList)
	{
		uiElement->CleanUp();
	}

	UIElementsList.clear();
	return true;
}


