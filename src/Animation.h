#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <SDL3/SDL_rect.h>

struct AnimFrame {
    SDL_Rect rect{};
    int durationMs = 100;
};

class Animation {
public:
    Animation();
    void AddFrame(const SDL_Rect& r, int durationMs);
    void SetLoop(bool v);
    void Reset();
    bool HasFinishedOnce() const;
    
    void Update(float dt);
    const SDL_Rect& GetCurrentFrame() const;
    int GetFrameCount() const;

private:
    std::vector<AnimFrame> frames_;
    int currentIndex_ = 0;
    int timeInFrameMs_ = 0;
    bool loop_ = true;
    bool finishedOnce_ = false;

    static SDL_Rect kEmpty_;
};

class AnimationSet {
public:
    AnimationSet();

    // load from TSX with aliases {baseTileId -> name}
    bool LoadFromTSX(const char* tsxPath,
        const std::unordered_map<int, std::string>& aliases);

    // ================== 【新增代码】开始 ==================
    // 允许手动添加动画，不需要 .tsx 文件
    void AddClip(const std::string& name, const Animation& anim) {
        clips_[name] = anim;
        // 如果是第一个添加的动画，默认设为当前动画，防止空指针
        if (currentName_.empty()) {
            currentName_ = name;
        }
    }
    // manage animations
    void SetCurrent(const std::string& name);
    void Update(float dtSeconds);
    const SDL_Rect& GetCurrentFrame() const;
    const std::string& GetCurrentName() const;
    bool HasFinishedOnce() const;
    bool Has(const std::string& name) const;

private:
    int tileW_ = 0, tileH_ = 0, columns_ = 0;
    std::unordered_map<std::string, Animation> clips_;
    std::string currentName_;

    static SDL_Rect TileIdToRect(int tileid, int columns, int tileW, int tileH);
};
