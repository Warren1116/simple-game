#include <imgui.h>
#include "Player.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ProjectileStraight.h"
#include "Collision.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include "SceneManager.h"
#include "SceneTitle.h"


// �R���X�g���N�^
Player::Player() {
    model = std::make_unique<Model>("Data/Model/Player/Rocket.mdl");
    // ���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.05f;
    position.y = 60.0f;
    position.z = -600.0f;
    angle.x = DirectX::XMConvertToRadians(90);

    hitEffect = std::make_unique<Effect>("Data/Effect/Explosion.efk");
    flyEffect = std::make_unique<Effect>("Data/Effect/engine0.1.efk");

    spriteGameclear = new Sprite("Data/Sprite/gameclear.png");
    spriteGameover = new Sprite("Data/Sprite/gameover.png");

}

// �f�X�g���N�^
Player::~Player() {
    delete spriteGameover;
    delete spriteGameclear;

}

// �X�V����
void Player::Update(float elapsedTime) {
    InputMove(elapsedTime);

    // ���͍X�V����
    UpdateVelocity(elapsedTime);

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
    if (IsDead() && ishitEnemy)
    {
        if (effectcount > 0)
        {
            hitEffect->Play(this->GetPosition());
            effectcount--;
        }

        timer--;    //count down 3sec
        if (timer <= 0)
        {
            DrawClear(dc);
            GamePad& gamePad = Input::Instance().GetGamePad();

            // �Ȃɂ��{�^�����������烍�[�f�B���O�V�[���֐؂�ւ�
            const GamePadButton anyButton =
                GamePad::BTN_A |
                GamePad::BTN_B |
                GamePad::BTN_X |
                GamePad::BTN_Y;

            if (gamePad.GetButtonDown() & anyButton) {
                SceneManager::Instance().ChangeScene(new SceneTitle);
            }
        }

    }
    else if (IsDead() && ishitEnemy == false)
    {
        timer--;    //count down 3sec
        if (timer <= 0)
        {
            DrawOver(dc);
            GamePad& gamePad = Input::Instance().GetGamePad();

            // �Ȃɂ��{�^�����������烍�[�f�B���O�V�[���֐؂�ւ�
            const GamePadButton anyButton =
                GamePad::BTN_A |
                GamePad::BTN_B |
                GamePad::BTN_X |
                GamePad::BTN_Y;

            if (gamePad.GetButtonDown() & anyButton) {
                SceneManager::Instance().ChangeScene(new SceneTitle);
            }
        }
    }


    if (model != nullptr)
    {
        shader->Draw(dc, model.get());

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
            ImGui::InputFloat("Acceleration", &acceleration);
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

// ���n�������ɌĂ΂��
void Player::OnLanding() {
    jumpCount = 0;
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
        this->acceleration += addSpeed;
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
    if (gamePad.GetButtonDown() & space)
    {
        fuelUse = true;
    }
    else if (gamePad.GetButtonUp() & space)
    {
        fuelUse = false;
    }
    if (fuelUse && fuel > 0)
    {
        DirectX::XMFLOAT3 pos = this->GetPosition();
        flyEffect->Play(pos);
    }
}

void Player::DrawClear(ID3D11DeviceContext* dc)
{
    spriteGameclear->Render(dc,
        300, 200, 1280 * 0.5f, 720 * 0.25,
        0, 0, 1270, 150,
        0,
        1, 1, 1, 1);
}

void Player::DrawOver(ID3D11DeviceContext* dc)
{
    spriteGameover->Render(dc,
        300, 200, 1280 * 0.5f, 720 * 0.25,
        0, 0, 1135, 150,
        0,
        1, 1, 1, 1);
}
