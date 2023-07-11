#pragma once

#include "Graphics/Sprite.h"
#include "Audio/Audio.h"
#include "Audio/AudioResource.h"
#include "Stage.h"
#include "Player.h"
#include "CameraController.h"
#include "Scene.h"

// ゲームシーン
class SceneGame : public Scene
{
public:
	SceneGame() {}
	~SceneGame() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
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
