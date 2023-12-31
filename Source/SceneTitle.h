#pragma once

#include "Graphics/Sprite.h"
#include "Audio/Audio.h"
#include "Audio/AudioResource.h"
#include "Scene.h"

class SceneTitle : public Scene {
public:
    SceneTitle() {}
    ~SceneTitle() override {}

    // 初期化
    void Initialize() override;

    // 終了化
    void Finalize() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render() override;

private:
    Sprite* sprite = nullptr;
    Sprite* black = nullptr;
    Model* model = nullptr;
    float fade = 0.0f;
    bool isFadein = false;

    std::unique_ptr<AudioSource>bgm = nullptr;
};