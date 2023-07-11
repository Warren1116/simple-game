#pragma once

#include "Graphics/Sprite.h"
#include "Audio/Audio.h"
#include "Audio/AudioResource.h"
#include "Scene.h"

class SceneTitle : public Scene {
public:
    SceneTitle() {}
    ~SceneTitle() override {}

    // ������
    void Initialize() override;

    // �I����
    void Finalize() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render() override;

private:
    Sprite* sprite = nullptr;
    Sprite* black = nullptr;
    Model* model = nullptr;
    float fade = 0.0f;
    bool isFadein = false;

    std::unique_ptr<AudioSource>bgm = nullptr;
};