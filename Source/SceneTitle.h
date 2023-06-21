#pragma once

#include "Graphics/Sprite.h"
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
};