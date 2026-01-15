#pragma once
#include "UIElement.h"

class UIToggle : public UIElement
{
public:
    UIToggle(int id, SDL_Rect bounds, const char* text, bool initialBufferState, Module* observer);
    virtual ~UIToggle();

    bool Update(float dt) override;

    // ????????
    bool IsOn() const { return isOn; }


    void SetTextures(SDL_Texture* textureOff, SDL_Texture* textureOn);

private:
    bool isOn; // true = ??, false = ??
    bool canClick = true;

    // 【新增】存储纹理
    SDL_Texture* texOff = nullptr; // 没选中时显示的图 (例如：显示暂停图标，表示游戏正在进行)
    SDL_Texture* texOn = nullptr;  // 选中时显示的图 (例如：显示播放图标，表示游戏已暂停)
};


