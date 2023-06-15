#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"

// 初期化
void SceneGame::Initialize()
{
	// ステージ初期化
	stage = new Stage();
	player = new Player();
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

	// エネミー初期化
	for (int i = 0; i < 2; ++i) {
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		EnemyManager::Instance().Register(slime);
	}
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
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);
	// ステージ更新処理
	stage->Update(elapsedTime);
	// プレイヤー更新処理
	player->Update(elapsedTime);
	// エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);
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

	}

	// 2DデバッグGUI描画
	{
		// プレイヤーデバッグ描画
		player->DrawDebugGUI();
	}
}
