#include "SceneManager.h"

// 更新処理
void SceneManager::Update(float elapsedTime) {
    if (nextScene != nullptr) {
        // 古いシーンを終了処理
        Clear();

        // 新しいシーンを設定
        currentScene = nextScene;
        nextScene = nullptr;

        // シーン初期化処理
        currentScene->Initialize();
    }

    if (currentScene != nullptr) {
        currentScene->Update(elapsedTime);
    }
}

// 描画処理
void SceneManager::Render() {
    if (currentScene != nullptr) {
        currentScene->Render();
    }
}

// シーンクリア
void SceneManager::Clear() {
    if (currentScene != nullptr) {
        currentScene->Finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}

// シーン切り替え
void SceneManager::ChangeScene(Scene* scene) {
    // 新しいシーンを設定
    nextScene = scene;
}