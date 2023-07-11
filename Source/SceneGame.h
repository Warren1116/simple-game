#pragma once

#include "Graphics/Sprite.h"
#include "Audio/Audio.h"
#include "Audio/AudioResource.h"
#include "Stage.h"
#include "Player.h"
#include "CameraController.h"
#include "Scene.h"

// �Q�[���V�[��
class SceneGame : public Scene
{
public:
	SceneGame() {}
	~SceneGame() override {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render() override;

private:
	Stage* stage = nullptr;
	Player* player = nullptr;
	CameraController* cameraController = nullptr;
	Sprite* sprite_gauge = nullptr;
	Sprite* sprite_in_gauge = nullptr;
	std::unique_ptr<AudioSource>bgm = nullptr;
	int stopTimer = 15;
};
