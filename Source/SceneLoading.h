#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include <thread>

// ローディングシーン
class SceneLoading : public Scene {
public:
    SceneLoading(Scene* nextScene) : nextScene(nextScene) {} // コンストラクタの初期化リストで次のシーンを設定
    ~SceneLoading() override {}

    // 初期化
    void Initialize() override;

    // 終了化
    void Finalize() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render() override;

private:
    // ローディングスレッド
    static void LoadingThread(SceneLoading* scene);

private:
    Sprite* sprite = nullptr;
    Sprite* Enter= nullptr;
    Sprite* tutorial[3];
    float angle = 0.0f;
    int tutorialNum = 0;
    int flashingTimer = 0;
    Scene* nextScene = nullptr;
    std::thread* thread = nullptr;
};