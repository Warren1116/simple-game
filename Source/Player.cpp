#include <imgui.h>
#include "Player.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ProjectileStraight.h"
#include "Collision.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include <cmath>

// �R���X�g���N�^
Player::Player() {
    model = new Model("Data/Model/Player/Missile.mdl");

    // ���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.08f;
    position.y = 1.0f;
    angle.x = DirectX::XMConvertToRadians(90);

    hitEffect = new Effect("Data/Effect/Explosion.efk");
    flyEffect = new Effect("Data/Effect/Explosion.efk");

}

// �f�X�g���N�^
Player::~Player() {
    delete hitEffect;
    delete flyEffect;
    delete model;
}

// �X�V����
void Player::Update(float elapsedTime) {
    InputMove(elapsedTime);

    // �W�����v���͏���
    //InputJump();

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
    if (model != nullptr)
    {
        model->UpdateTransform(transform);
    }
}

// �`�揈��
void Player::Render(ID3D11DeviceContext* dc, Shader* shader) {
    if (model != nullptr)
    {
        shader->Draw(dc, model);

        // �e�ە`�揈��
        projectileManager.Render(dc,shader);

    }
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
        if (ImGui::CollapsingHeader("FuelDebug", ImGuiTreeNodeFlags_DefaultOpen)) {
            // AddSpeed
            ImGui::InputFloat("AddSpeed", &addSpeedEnergy);
            // SubSpeed
            ImGui::InputFloat("SubSpeed", &subSpeedEnergy);
            // Fuel
            ImGui::InputFloat("Fuel", &fuel);
            // SubFuelSpeed
            ImGui::InputFloat("SubFuelSpeed", &subFuelEnergy);
        }


    }
    ImGui::End();
}

// �X�e�B�b�N���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec() const {
    // �{���͑O���x�N�g�������o����90�x��]�����Ă���̂ŏ�����x�N�g�������Ă���
    DirectX::XMFLOAT3 vec = { transform._21, transform._22, transform._23 };

    return vec;
}

// �ړ����͏���
void Player::InputMove(float elapsedTime) {
    // �i�s�x�N�g���擾
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // �R�����g���Ă��邩
    ChackUseFuel();

    // ���x�����邩
    ChackHasSpeed();

    // ���x��������
    ChackMoveSpeed(elapsedTime);

    // �ړ�����
    MoveFront(moveVec, moveSpeed);

    // ���͉�]����
    if (hasSpeed) InputTurn(elapsedTime, moveVec, turnSpeed);
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
            }
            // �����o����̈ʒu�ݒ�
            else enemy->SetPosition(outPosition);
            hitEffect->Play(this->GetPosition());
            model = nullptr;
            
        }
    }
}

// �W�����v���͏���
//void Player::InputJump() {
//    // �{�^�����͂ŃW�����v(�W�����v�񐔐�����)
//    GamePad& gamePad = Input::Instance().GetGamePad();
//    if (gamePad.GetButtonDown() & GamePad::BTN_A) {
//        if (jumpCount < jumpLimit) {
//            Jump(jumpSpeed);
//            jumpCount++;
//        }
//    }
//}

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

// �O�i����
void Player::MoveFront(DirectX::XMFLOAT3 direction, float speed) {
    // ��ɑO�i����

    moveVecX += direction.x;
    moveVecY += direction.y;
    moveVecZ += direction.z;

    maxMoveSpeed = speed;
    //flyEffect->Play(this->GetPosition());
}

// �����ύX
void Player::InputTurn(float elapsedTime, DirectX::XMFLOAT3 direction, float speed) {
    speed *= elapsedTime;

    // ���͏����擾
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    //// ���ϒl��-1.0�`1.0�ŕ\������Ă���A2�̒P�ʃx�N�g���̊p�x��
    //// �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
    //float rot = 1.0 - dot; // �␳�l
    //if (rot > speed) rot = speed;

    angle.x -= ay * speed;
    angle.y += ax * speed;

    //if (angle.y < AngleMinY) {
    //    angle.y = AngleMinY;
    //}

    //if (angle.y > AngleMaxY) {
    //    angle.y = AngleMaxY;
    //}

    //if (angle.x <= AngleMinX) {
    //    angle.x = AngleMinX;
    //}

    //if (angle.x >= AngleMaxX) {
    //    angle.x = AngleMaxX;
    //}
}

void Player::ChackMoveSpeed(float elapsedTime) {
    float addSpeed = addSpeedEnergy * elapsedTime;
    float subSpeed = subSpeedEnergy * elapsedTime;
    float subFuel = subFuelEnergy * elapsedTime;
    if (fuelUse && fuel > 0) {
        moveSpeed += addSpeed;
        fuel -= subFuel;
    }
    else {
        moveSpeed -= subSpeed;
        if (moveSpeed < 0) {
            moveSpeed = 0;
        }
    }
}

void Player::ChackHasSpeed() {
    if (moveSpeed > 0) hasSpeed = true;
    else hasSpeed = false;

}

void Player::ChackUseFuel() {
    GamePad& gamePad = Input::Instance().GetGamePad();
    const GamePadButton space = GamePad::BTN_SPACE;
    if (gamePad.GetButtonDown() & space) fuelUse = true;
    else if (gamePad.GetButtonUp() & space) fuelUse = false;
}