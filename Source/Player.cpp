#include <imgui.h>
#include "Player.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTitle.h"
#include "SceneGame.h"


// �R���X�g���N�^
Player::Player() {
    model = std::make_unique<Model>("Data/Model/Player/Rocket.mdl");

    // ���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.05f;
    position.y = 60.0f;
    position.z = -600.0f;
    angle.x = DirectX::XMConvertToRadians(90);
    height = 0.5f;
    //hitEffect = std::make_unique<Effect>("Data/Effect/Explosion.efk");
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
    if (model!= nullptr)
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

        deathTimer--;    //count down 3sec
        if (deathTimer <= 0)
        {
            DrawClear(dc);
            GamePad& gamePad = Input::Instance().GetGamePad();

            // �Ȃɂ��{�^�����������烍�[�f�B���O�V�[���֐؂�ւ�
            const GamePadButton anyButton = GamePad::BTN_ENTER;

            if (gamePad.GetButtonDown() & anyButton) {
                SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
            }
        }

    }
    else if (IsDead() && ishitEnemy == false)
    {
        if (effectcount > 0)
        {
            hitEffect->Play(this->GetPosition());
            effectcount--;
        }
        deathTimer--;    //count down 3sec
        if (deathTimer <= 0)
        {
            DrawOver(dc);
            GamePad& gamePad = Input::Instance().GetGamePad();

            // �Ȃɂ��{�^�����������烍�[�f�B���O�V�[���֐؂�ւ�
            const GamePadButton anyButton = GamePad::BTN_ENTER;

            if (gamePad.GetButtonDown() & anyButton) {
                SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
            }
        }
    }


    if (model != nullptr)
    {
        shader->Draw(dc, model.get());

    }

}

// �f�o�b�O�v���~�e�B�u�`��
void Player::DrawDebugPrimitive() {
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // �Փ˔���p�̃f�o�b�O����`��
    debugRenderer->DrawCylinder({ position.x, position.y, position.z }, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}

// �f�o�b�O�pGUI�`��
void Player::DrawDebugGUI() {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None)) {
        // �g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform")) {
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
        if (ImGui::CollapsingHeader("Movement")) {
            // MoveSpeed
            ImGui::InputFloat("Acceleration", &acceleration);
            // MaxAcceleSpeed
            ImGui::InputFloat("MaxAcceleSpeed", &MaxAcceleSpeed);
            // MinAcceleSpeed
            ImGui::InputFloat("MinAcceleSpeed", &MinAcceleSpeed);
            // TurnSpeed
            ImGui::InputFloat("TurnSpeed", &turnSpeed);
            // Velocity
            ImGui::InputFloat3("Velocity", &velocity.x);
            // MinVelocityZ
            ImGui::InputFloat("MinVelocityZ", &MinVelocityZ);
        }
        if (ImGui::CollapsingHeader("FuelDebug")) {
            // AddSpeed
            ImGui::InputFloat("AddAcceleSpeed", &addSpeedEnergy);
            // SubSpeed
            ImGui::InputFloat("SubAcceleSpeed", &subSpeedEnergy);
            // Fuel
            ImGui::InputFloat("Fuel", &fuel);
            // SubFuelSpeed
            ImGui::InputFloat("SubFuelSpeed", &subFuelEnergy);
        }
        if (ImGui::CollapsingHeader("DeathParameter")) {
            float dyingAx;
            dyingAx = DirectX::XMConvertToDegrees(DyingAngleX);
            // DeathFallAdjustment
            ImGui::InputFloat("DeathFallAdjustment", &DeathFallAdjustment);
            // DeathTimer
            ImGui::InputInt("DeathTimer", &deathTimer);
            // DyingAngleX
            ImGui::InputFloat("DyingAngleX", &dyingAx);
            DyingAngleX = DirectX::XMConvertToRadians(dyingAx);
            // DeathRotateAdjustment
            ImGui::InputFloat("DeathRotateAdjustment", &DeathRotateAdjustment);
        }
        if (ImGui::CollapsingHeader("GravityParameter")) {
            // Gravity
            ImGui::InputFloat("Gravity", &gravity);
            // GravityYAdjustmentNormal
            ImGui::InputFloat("GravityYAdjustmentNormal", &GravityYAdjustmentNormal);
            // GravityYAdjustmentMinVelocityZ
            ImGui::InputFloat("GravityYAdjustmentMinVelocityZ", &GravityYAdjustmentMinVelocityZ);
            // GravityZAdjustmentAdd
            ImGui::InputFloat("GravityZAdjustmentAdd", &GravityZAdjustmentAdd);
            // GravityZAdjustmentSub
            ImGui::InputFloat("GravityZAdjustmentSub", &GravityZAdjustmentSub);
        }
        if (ImGui::CollapsingHeader("RotationParameter")) {
            float ayMax, ayMin, axMax, axMin, turnSpeed;
            ayMax = DirectX::XMConvertToDegrees(AngleMaxY);
            ayMin = DirectX::XMConvertToDegrees(AngleMinY);
            axMax = DirectX::XMConvertToDegrees(AngleMaxX);
            axMin = DirectX::XMConvertToDegrees(AngleMinX);
            // AngleMaxY
            ImGui::InputFloat("AngleMaxY", &ayMax);
            AngleMaxY = DirectX::XMConvertToRadians(ayMax);
            // AngleMinY
            ImGui::InputFloat("AngleMinY", &ayMin);
            AngleMinY = DirectX::XMConvertToRadians(ayMin);
            // AngleMaxX
            ImGui::InputFloat("AngleMaxX", &axMax);
            AngleMaxX = DirectX::XMConvertToRadians(axMax);
            // AngleMinX
            ImGui::InputFloat("AngleMinX", &axMin);
            AngleMinX = DirectX::XMConvertToRadians(axMin);
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

    if(!isDead)
    // ���x��������
    ChangeSpeed(elapsedTime);

    // �ړ�����
    MoveFront(moveVec, moveSpeed);

    // ���͉�]����
    InputTurn(elapsedTime, moveVec, turnSpeed);
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
            { this->GetPosition().x, this->GetPosition().y + 0.25f ,this->GetPosition().z }, this->GetRadius(), this->GetHeight(),
            { enemy->GetPosition().x, enemy->GetPosition().y, enemy->GetPosition().z }, enemy->GetRadius(), enemy->GetHeight(),
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
            enemy->CheckWallCollision();
            model = nullptr;
            isDead = true;
            ishitEnemy = true;
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

    if (/*hasSpeed*/fuel > 0) {
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

        if (angle.y < AngleMinY) {
            angle.y = AngleMinY;
        }

        if (angle.y > AngleMaxY) {
            angle.y = AngleMaxY;
        }

        // �㉺����
        if (angle.x <= AngleMinX) {
            angle.x = AngleMinX;
        }

        if (angle.x >= AngleMaxX) {
            angle.x = AngleMaxX;
        }
    }
    else {
        if (angle.x >= DirectX::XMConvertToRadians(45)) {
            angle.x += speed * DeathRotateAdjustment;
            if (angle.x >= DyingAngleX) angle.x = DyingAngleX;
        }
    }
}

void Player::ChangeSpeed(float elapsedTime) {
    if (fuelUse && fuel > 0) {
        float subFuel = subFuelEnergy * elapsedTime;
        fuel -= subFuel;
        onSpeed = true;
        float addSpeed = addSpeedEnergy * elapsedTime;
        acceleration += addSpeed;
        if (acceleration > MaxAcceleSpeed) acceleration = MaxAcceleSpeed;
        if (fuel < 0) {
            fuel = 0;
            onSpeed = false;
        }
    }
    else {
        if (onSpeed) {
            float subSpeed = subSpeedEnergy * elapsedTime;
            acceleration += subSpeed;
            if (acceleration < MinAcceleSpeed) acceleration = MinAcceleSpeed;
        }
    }
}

void Player::ChackHasSpeed() {
    if (acceleration > MinAcceleSpeed) {
        hasSpeed = true;
        deathSpeed = 0.0f;
    }
    else {
        hasSpeed = false;
    }
}

void Player::ChackUseFuel() {
    GamePad& gamePad = Input::Instance().GetGamePad();
    const GamePadButton space = GamePad::BTN_SPACE;

    if (gamePad.GetButtonDown() & space) fuelUse = true;
    else if (gamePad.GetButtonUp() & space) fuelUse = false;

    if (fuelUse && fuel > 0 && IsDead() == false)
    {
        //onAddSpeed = true;
        DirectX::XMFLOAT3 pos = this->GetPosition();
        flyEffect->Play(pos);
    }
}

void Player::DrawClear(ID3D11DeviceContext* dc)
{
    if (showHeight != screenHeight * 0.25)
    {
        showHeight += 2.5f;
    }
    spriteGameclear->Render(dc,
        screenWidth * 0.35f, showHeight, screenWidth * 0.5f, 200,
        0, 0, 1270, 150,
        0,
        1, 1, 1, 1);
}

void Player::DrawOver(ID3D11DeviceContext* dc)
{
    if (showHeight != screenHeight * 0.25)
    {
        showHeight += 2.5f;
    }
    spriteGameover->Render(dc,
        screenWidth * 0.35f, showHeight, screenWidth * 0.5f, 200,
        0, 0, 1135, 150,
        0,
        1, 1, 1, 1);
}

void Player::UpdateVerticalVelocity(float elapsedFrame) {
    // �R�����g���Ă��Ȃ���Ώc�ɏd�͂�������
    if (!fuelUse) {
        if (hasSpeed && fuel > 0) {
            float speed = gravity * elapsedFrame;

            if (velocity.z > MinVelocityZ) velocity.y += speed * GravityYAdjustmentNormal;
            else if (velocity.z <= MinVelocityZ) velocity.y += speed * GravityYAdjustmentMinVelocityZ;
        }
        else {
            deathSpeed += gravity * elapsedFrame;
            velocity.y += deathSpeed * DeathFallAdjustment;
        }
    }
}

void Player::UpdateHorizontalVelocity(float elapsedFrame) {

    if (!fuelUse) GravityAdjust(elapsedFrame);

    //XZ���ʂ̑��͂���������
    float length = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);

    if (length > 0.0f)
    {
        //���C��
        float friction = this->friction * elapsedFrame;

        friction *= airControl;

        //���C�ɂ�鉡�����̌�������
        if (length > friction)
        {
            //�P�ʃx�N�g����
            float vx = velocity.x / length;
            float vy = velocity.y / length;
            float vz = velocity.z / length;

            velocity.x -= vx * friction;
            velocity.y -= vy * friction;
            velocity.z -= vz * friction;
            if (velocity.z < MinVelocityZ) velocity.z = MinVelocityZ;
        }
        //�������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌���
        else
        {
            velocity.x = 0.0f;
            velocity.y = 0.0f;
            velocity.z = MinVelocityZ;
        }
    }
    //XZ���ʂ̑��͂���������
    if (length <= maxMoveSpeed)
    {
        float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecY * moveVecY + moveVecZ * moveVecZ);

        if (moveVecLength > 0.0f)
        {
            float acceleration = this->acceleration * elapsedFrame;

            velocity.x += moveVecX * acceleration;
            velocity.y += moveVecY * acceleration;
            velocity.z += moveVecZ * acceleration;

            float length = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
            if (length > maxMoveSpeed)
            {
                float vx = velocity.x / length;
                float vy = velocity.y / length;
                float vz = velocity.z / length;
                velocity.x = vx * maxMoveSpeed;
                velocity.y = vy * maxMoveSpeed;
                velocity.z = vz * maxMoveSpeed;
            }
        }
    }
    moveVecX = 0.0f;
    moveVecY = 0.0f;
    moveVecZ = 0.0f;
}

// �d�͂ɂ�������
void Player::GravityAdjust(float elapsedFrame) {

    float rotate = sinf(angle.x - DirectX::XMConvertToRadians(90)); // ���W�b�N�̒���90�x�����Ă�̂͂悭�Ȃ�
    float speed = gravity * elapsedFrame;
    float rot = 1.0f - rotate * 0.5f/* - rotate*/; // ���������_
    // ������
    //if (rotate > 0 && onAddSpeed) velocity.z -= speed * rot * GravityZAdjustmentAdd;
    // �����
    //else if (rotate < 0 && velocity.z > MinVelocityZ) velocity.z += speed * rot * GravityZAdjustmentSub;  

    // ������
    if (rotate > DirectX::XMConvertToRadians(0)) {
        onSpeed = true;
        acceleration += -speed * rot * 0.2f;
    }
    //else if (rotate == DirectX::XMConvertToRadians(0)) onSpeed = false;
    // �����
    else/* if (rotate < 0 && acceleration > MinAcceleSpeed)*/ {
        onSpeed = false;
        acceleration -= -speed * rot * 0.2f;
        if (acceleration < MinAcceleSpeed) acceleration = MinAcceleSpeed;
    }
}