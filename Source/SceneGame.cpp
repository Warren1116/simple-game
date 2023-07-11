#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"

// 初期化
void SceneGame::Initialize()
{
	// スプライト初期化
	sprite_gauge = new Sprite("Data/Sprite/gauge.png");
	sprite_in_gauge = new Sprite("Data/Sprite/In_gauge.png");
	// ステージ初期化
	stage = new Stage();
	player = new Player();
	// BGM初期化
	bgm = Audio::Instance().LoadAudioSource("Data/Audio/game.wav");
	bgm->Play(true);
	// カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);
	// カメラコントローラー初期化
	cameraController = new CameraController();
	cameraController->SetCameraType(cameraController->CameraType1);

	// エネミー初期化

	EnemySlime* slime = new EnemySlime();
	slime->SetPosition(DirectX::XMFLOAT3(50.0f, 118.0f - 12.0f, 382.0f));
	EnemyManager::Instance().Register(slime);

}

// 終了化
void SceneGame::Finalize()
{
	// エネミー終了化
	EnemyManager::Instance().Clear();
	// カメラコントローラー終了化
	if (cameraController != nullptr) {
		delete cameraController;
		cameraController = nullptr;
	}
	if (stage != nullptr) {
		delete stage;
		stage = nullptr;
	}
	if (player != nullptr) {
		delete player;
		player = nullptr;
	}
	if (sprite_gauge != nullptr) {
		delete sprite_gauge;
		sprite_gauge = nullptr;
	}
	if (sprite_in_gauge != nullptr) {
		delete sprite_in_gauge;
		sprite_in_gauge = nullptr;
	}
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	// 死亡時少し止める
	{
		if (player->GetStop()) 
		{
			stopTimer--;
			if (stopTimer > 0) return;
		}
	}
	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	// 死亡時画面揺らし
	if (player->GetDead())
	{
		int ShakeTimer = -1 * stopTimer;
		if (ShakeTimer % 5 == 0 && ShakeTimer < 21)
		{
			target.x += 1.0f;
			target.y += 1.0f;
		}
	}
	cameraController->SetTarget(target);
	if (cameraController->GetCameraType() == cameraController->CameraType2) {
		DirectX::XMFLOAT3 direction = { player->GetTransform()._21, player->GetTransform()._22,player->GetTransform()._23 };
		cameraController->SetTDirection(direction);
	}

	cameraController->Update(elapsedTime);
	// ステージ更新処理
	stage->Update(elapsedTime);
	// プレイヤー更新処理
	player->Update(elapsedTime);
	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);

	EffectManager::Instance().Update(elapsedTime);
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		// ステージ描画
		stage->Render(dc, shader);
		player->Render(dc, shader);
		EnemyManager::Instance().Render(dc, shader);
		shader->End(dc);
	}

	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3Dデバッグ描画
	{
		// プレイヤーデバッグプリミティブ描画
		player->DrawDebugPrimitive();

		// エネミーデバッグプリミティブ描画
		EnemyManager::Instance().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2Dスプライト描画
	{
		float screenWidth = static_cast<float>(1280.0f);
		float screenHeight = static_cast<float>(720.0f);
		float textureWidth = static_cast<float>(sprite_gauge->GetTextureWidth());
		float textureHeight = static_cast<float>(sprite_gauge->GetTextureHeight());
		// ゲージスプライト描画
		sprite_gauge->Render(dc,
			screenWidth * 0.3f, screenHeight, 700, 45,
			0, 0, textureWidth, textureHeight,
			0,
			1, 1, 1, 1);

		if (player->GetFuel() >= 0.0f)
		{
			float screenWidth = static_cast<float>(1280.0f);
			float screenHeight = static_cast<float>(720.0f);
			textureWidth = static_cast<float>(sprite_in_gauge->GetTextureWidth());
			textureHeight = static_cast<float>(sprite_in_gauge->GetTextureHeight());
			// ゲージスプライト描画
			sprite_in_gauge->Render(dc,
				screenWidth * 0.3f + 5, screenHeight + 3, (player->GetFuel() / 100) * 690.0f, 39,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}
	}

	// 2DデバッグGUI描画
	{
		// プレイヤーデバッグ描画
		player->DrawDebugGUI();
		cameraController->DrawDebugGUI();
	}
}
