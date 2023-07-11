#pragma once

#include "Graphics/Sprite.h"
#include "Audio/Audio.h"
#include "Audio/AudioResource.h"
#include "Scene.h"

class SceneTitle : public Scene {
public:
    SceneTitle() {}
    ~SceneTitle() override {}

    // ‰Šú‰»
    void Initialize() override;

    // I—¹‰»
    void Finalize() override;

    // XVˆ—
    void Update(float elapsedTime) override;

    // •`‰æˆ—
    void Render() override;

private:
    Sprite* sprite = nullptr;
    Model* model = nullptr;
    std::unique_ptr<AudioSource>bgm = nullptr;
};