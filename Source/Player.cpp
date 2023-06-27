#include <imgui.h>
#include "Player.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ProjectileStraight.h"
#include "Collision.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"

// �R���X�g���N�^
Player::Player() {
    model = new Model("Data/Model/Player/Missile.mdl");

    // ���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.1f;


    position.y = 0.5f;
    
    angle.x = DirectX::XMConvertToRadians(90);

    hitEffect = new Effect("Data/Effect/Explosion.efk");

}

// �f�X�g���N�^
Player::~Player() {
    delete hitEffect;
    delete model;
}

// �X�V����
void Player::Update(float elapsedTime) {
    InputMove(elapsedTime);

    // �W�����v���͏���
    InputJump();

    // �e�ۓ��͏���
    InputProjectile();

    // ���͍X�V����
    UpdateVelocity(elapsedTime);

    // �e�ۍX�V����
    projectileManager.Update(elapsedTime);

    // �v���C���[�ƓG�Ƃ̏Փˏ���
    CollisionPlayerVsEnemies();

    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();
    // ���f���s��X�V
    model->UpdateTransform(transform);
}

// �`�揈��
void Player::Render(ID3D11DeviceContext* dc, Shader* shader) {
    shader->Draw(dc, model);

    // �e�ە`�揈��
    projectileManager.Render(dc,shader);
}

// �f�o�b�O�v���~�e�B�u�`��
void Player::DrawDebugPrimitive() {
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // �Փ˔���p�̃f�o�b�O����`��
    debugRenderer->DrawCylinder(position, radius,height, DirectX::XMFLOAT4(0, 0, 0, 1));

    // �e�ۃf�o�b�O�v���~�e�B�u�`��
    projectileManager.DrawDebugPrimitive();
}

// �f�o�b�O�pGUI�`��
void Player::DrawDebugGUI() {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None)) {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            // �ʒu
            ImGui::InputFloat3("Position", &position.x);
            // ��]
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);
            // �X�P�[��
            ImGui::InputFloat3("Scale", &scale.x);
        }
        if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_DefaultOpen)) {
            // MoveSpeed
            ImGui::InputFloat("MoveSpeed", &moveSpeed);
            // TurnSpeed
            ImGui::InputFloat("TurnSpeed", &turnSpeed);
            // JumpSpeed
            ImGui::InputFloat("JumpSpeed", &jumpSpeed);
            // Gravity
            ImGui::InputFloat("Gravity", &gravity);
            // Velocity
            ImGui::InputFloat3("Velocity", &velocity.x);
        }
        if (ImGui::CollapsingHeader("Collision", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputFloat("Radius", &radius);
        }
    }
    ImGui::End();
}

// �X�e�B�b�N���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec() const {
    // ���͏����擾
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    // �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    // �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

    // �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);

    if (cameraRightLength > 0.0f) {
        // �P�ʃx�N�g����
        cameraRightX = cameraRightX / cameraRightLength;
        cameraRightZ = cameraRightZ / cameraRightLength;
    }

    // �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);

    if (cameraFrontLength > 0.0f) {
        // �P�ʃx�N�g����
        cameraFrontX = cameraFrontX / cameraFrontLength;
        cameraFrontZ = cameraFrontZ / cameraFrontLength;
    }

    // �X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
    // �X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
    // �i�s�x�N�g�����v�Z����
    DirectX::XMFLOAT3 vec;
    vec.x = cameraRightX * ax + cameraFrontX * ay; // �x�N�g����
    vec.z = cameraRightZ * ax + cameraFrontZ * ay;

    // Y�������ɂ͈ړ����Ȃ�
    vec.y = 0.0f;

    return vec;
}

// �ړ����͏���
void Player::InputMove(float elapsedTime) {
    // �i�s�x�N�g���擾
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // �ړ�����
    Move(moveVec.x, moveVec.z, moveSpeed);

    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

void Player::CollisionPlayerVsEnemies() {
    EnemyManager& enemyManager = EnemyManager::Instance();

    // �S�Ă̓G�Ƒ�������ŏՓˏ���
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; i++) {
        Enemy* enemy = enemyManager.GetEnemy(i);

        // �Փˏ���
        DirectX::XMFLOAT3 outPosition;
        if (Collision::IntersectCylinderVsCylinder (
            this->GetPosition(), this->GetRadius(),this->GetHeight(),
            enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(),
            outPosition)) {
            float enemyHeight = enemy->GetPosition().y + enemy->GetHeight();
            float playerHeight = this->GetPosition().y + this->GetHeight();
            float length = playerHeight - enemyHeight; // �v���C���[�̓���ƓG�̓���̃x�N�g�������
            if (this->GetPosition().y > enemyHeight - length) {
                velocity.y = 0;
                Jump(10);
            }
            // �����o����̈ʒu�ݒ�
            else enemy->SetPosition(outPosition);
            hitEffect->Play(this->GetPosition());
        }
    }
}

// �W�����v���͏���
void Player::InputJump() {
    // �{�^�����͂ŃW�����v(�W�����v�񐔐�����)
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A) {
        if (jumpCount < jumpLimit) {
            Jump(jumpSpeed);
            jumpCount++;
        }
    }
}

// ���n�������ɌĂ΂��
void Player::OnLanding() {
    jumpCount = 0;
}

void Player::InputProjectile() {
    GamePad& gamePad = Input::Instance().GetGamePad();

    // ���i�e�۔���
    if (gamePad.GetButtonDown() & GamePad::BTN_X) {
        // �O����
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.x);
        dir.y = 0;
        dir.z = cosf(angle.z);
        // ���ˈʒu (�v���C���[�̍�������)
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height * 0.5f;
        pos.z = position.z;
        // ����
        ProjectileStraight* projectile = new ProjectileStraight (&projectileManager);
        projectile->Launch(dir, pos);
    }
}
