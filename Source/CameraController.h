#pragma once

#include <DirectXMath.h>

// �J�����R���g���[���[
class CameraController {
public:
	CameraController () {}
	~CameraController () {}

	// �X�V����
	void Update(float elapsedTime);

	// �^�[�Q�b�g�ʒu�ݒ�
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	// �A���O���ݒ�
	void SetTDirection(const DirectX::XMFLOAT3& tDirection) { this->targetDirection = tDirection; }

	// �f�o�b�O�p�J�����^�C�v�̐ݒ�
	void SetCameraType(int cameraType) { this->cameraType = cameraType; }

	// �f�o�b�O�p�J�����^�C�v�̎擾
	int GetCameraType() { return cameraType; }

	// �f�o�b�O�pGUI
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
	// �萔
	const int CameraType1 = 0;
	const int CameraType2 = 1;
};