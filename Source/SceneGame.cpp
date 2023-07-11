#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"

// ������
void SceneGame::Initialize()
{
	// �X�v���C�g������
	sprite_gauge = new Sprite("Data/Sprite/gauge.png");
	sprite_in_gauge = new Sprite("Data/Sprite/In_gauge.png");
	// �X�e�[�W������
	stage = new Stage();
	player = new Player();
	// BGM������
	bgm = Audio::Instance().LoadAudioSource("Data/Audio/game.wav");
	bgm->Play(true);
	// �J���������ݒ�
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
	// �J�����R���g���[���[������
	cameraController = new CameraController();
	cameraController->SetCameraType(cameraController->CameraType1);

	// �G�l�~�[������

	EnemySlime* slime = new EnemySlime();
	slime->SetPosition(DirectX::XMFLOAT3(50.0f, 118.0f - 12.0f, 382.0f));
	EnemyManager::Instance().Register(slime);

}

// �I����
void SceneGame::Finalize()
{
	// �G�l�~�[�I����
	EnemyManager::Instance().Clear();
	// �J�����R���g���[���[�I����
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

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	// ���S�������~�߂�
	{
		if (player->GetStop()) 
		{
			stopTimer--;
			if (stopTimer > 0) return;
		}
	}
	// �J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	// ���S����ʗh�炵
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
	// �X�e�[�W�X�V����
	stage->Update(elapsedTime);
	// �v���C���[�X�V����
	player->Update(elapsedTime);
	// �G�l�~�[�X�V����
	EnemyManager::Instance().Update(elapsedTime);

	EffectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// �`�揈��
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ���C�g�����i�������j

	// �J�����p�����[�^�ݒ�
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3D���f���`��
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		// �X�e�[�W�`��
		stage->Render(dc, shader);
		player->Render(dc, shader);
		EnemyManager::Instance().Render(dc, shader);
		shader->End(dc);
	}

	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}

	// 3D�f�o�b�O�`��
	{
		// �v���C���[�f�o�b�O�v���~�e�B�u�`��
		player->DrawDebugPrimitive();

		// �G�l�~�[�f�o�b�O�v���~�e�B�u�`��
		EnemyManager::Instance().DrawDebugPrimitive();

		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2D�X�v���C�g�`��
	{
		float screenWidth = static_cast<float>(1280.0f);
		float screenHeight = static_cast<float>(720.0f);
		float textureWidth = static_cast<float>(sprite_gauge->GetTextureWidth());
		float textureHeight = static_cast<float>(sprite_gauge->GetTextureHeight());
		// �Q�[�W�X�v���C�g�`��
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
			// �Q�[�W�X�v���C�g�`��
			sprite_in_gauge->Render(dc,
				screenWidth * 0.3f + 5, screenHeight + 3, (player->GetFuel() / 100) * 690.0f, 39,
				0, 0, textureWidth, textureHeight,
				0,
				1, 1, 1, 1);
		}
	}

	// 2D�f�o�b�OGUI�`��
	{
		// �v���C���[�f�o�b�O�`��
		player->DrawDebugGUI();
		cameraController->DrawDebugGUI();
	}
}
