#pragma once

#include <DirectXMath.h>

// カメラコントローラー
class CameraController {
public:
	CameraController () {}
	~CameraController () {}

	// 更新処理
	void Update(float elapsedTime);

	// ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	// アングル設定
	void SetTDirection(const DirectX::XMFLOAT3& tDirection) { this->targetDirection = tDirection; }

	// デバッグ用カメラタイプの設定
	void SetCameraType(int cameraType) { this->cameraType = cameraType; }

	// デバッグ用カメラタイプの取得
	int GetCameraType() { return cameraType; }

	// デバッグ用GUI
	void DrawDebugGUI();

private:
	DirectX::XMFLOAT3 target = { 0,0,0 };
	DirectX::XMFLOAT3 targetDirection = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	float rollSpeed = DirectX::XMConvertToRadians(90);
	float range = 4.0f;
	float maxAngleX = DirectX::XMConvertToRadians(35);
	float minAngleX = DirectX::XMConvertToRadians(-25);
	int cameraType = CameraType1;
public:
	// 定数
	const int CameraType1 = 0;
	const int CameraType2 = 1;
};