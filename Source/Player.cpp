#include <imgui.h>
#include "Player.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ProjectileStraight.h"
#include "Collision.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"

// コンストラクタ
Player::Player() {
    model = new Model("Data/Model/Player/Missile.mdl");

    // モデルが大きいのでスケーリング
    scale.x = scale.y = scale.z = 0.1f;


    position.y = 0.5f;
    
    angle.x = DirectX::XMConvertToRadians(90);

    hitEffect = new Effect("Data/Effect/Explosion.efk");

}

// デストラクタ
Player::~Player() {
    delete hitEffect;
    delete model;
}

// 更新処理
void Player::Update(float elapsedTime) {
    InputMove(elapsedTime);

    // ジャンプ入力処理
    InputJump();

    // 弾丸入力処理
    InputProjectile();

    // 速力更新処理
    UpdateVelocity(elapsedTime);

    // 弾丸更新処理
    projectileManager.Update(elapsedTime);

    // プレイヤーと敵との衝突処理
    CollisionPlayerVsEnemies();

    // オブジェクト行列を更新
    UpdateTransform();
    // モデル行列更新
    model->UpdateTransform(transform);
}

// 描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader) {
    shader->Draw(dc, model);

    // 弾丸描画処理
    projectileManager.Render(dc,shader);
}

// デバッグプリミティブ描画
void Player::DrawDebugPrimitive() {
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // 衝突判定用のデバッグ球を描画
    debugRenderer->DrawCylinder(position, radius,height, DirectX::XMFLOAT4(0, 0, 0, 1));

    // 弾丸デバッグプリミティブ描画
    projectileManager.DrawDebugPrimitive();
}

// デバッグ用GUI描画
void Player::DrawDebugGUI() {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None)) {
        // トランスフォーム
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            // 位置
            ImGui::InputFloat3("Position", &position.x);
            // 回転
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);
            // スケール
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

// スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const {
    // 入力情報を取得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    // カメラ方向とスティックの入力値によって進行方向を計算する
    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    // 移動ベクトルはXZ平面に水平なベクトルになるようにする

    // カメラ右方向ベクトルをXZ単位ベクトルに変換
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);

    if (cameraRightLength > 0.0f) {
        // 単位ベクトル化
        cameraRightX = cameraRightX / cameraRightLength;
        cameraRightZ = cameraRightZ / cameraRightLength;
    }

    // カメラ前方向ベクトルをXZ単位ベクトルに変換
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);

    if (cameraFrontLength > 0.0f) {
        // 単位ベクトル化
        cameraFrontX = cameraFrontX / cameraFrontLength;
        cameraFrontZ = cameraFrontZ / cameraFrontLength;
    }

    // スティックの水平入力値をカメラ右方向に反映し、
    // スティックの垂直入力値をカメラ前方向に反映し、
    // 進行ベクトルを計算する
    DirectX::XMFLOAT3 vec;
    vec.x = cameraRightX * ax + cameraFrontX * ay; // ベクトルを
    vec.z = cameraRightZ * ax + cameraFrontZ * ay;

    // Y軸方向には移動しない
    vec.y = 0.0f;

    return vec;
}

// 移動入力処理
void Player::InputMove(float elapsedTime) {
    // 進行ベクトル取得
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // 移動処理
    Move(moveVec.x, moveVec.z, moveSpeed);

    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

void Player::CollisionPlayerVsEnemies() {
    EnemyManager& enemyManager = EnemyManager::Instance();

    // 全ての敵と総当たりで衝突処理
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; i++) {
        Enemy* enemy = enemyManager.GetEnemy(i);

        // 衝突処理
        DirectX::XMFLOAT3 outPosition;
        if (Collision::IntersectCylinderVsCylinder (
            this->GetPosition(), this->GetRadius(),this->GetHeight(),
            enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(),
            outPosition)) {
            float enemyHeight = enemy->GetPosition().y + enemy->GetHeight();
            float playerHeight = this->GetPosition().y + this->GetHeight();
            float length = playerHeight - enemyHeight; // プレイヤーの頭上と敵の頭上のベクトルを作る
            if (this->GetPosition().y > enemyHeight - length) {
                velocity.y = 0;
                Jump(10);
            }
            // 押し出し後の位置設定
            else enemy->SetPosition(outPosition);
            hitEffect->Play(this->GetPosition());
        }
    }
}

// ジャンプ入力処理
void Player::InputJump() {
    // ボタン入力でジャンプ(ジャンプ回数制限つき)
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A) {
        if (jumpCount < jumpLimit) {
            Jump(jumpSpeed);
            jumpCount++;
        }
    }
}

// 着地した時に呼ばれる
void Player::OnLanding() {
    jumpCount = 0;
}

void Player::InputProjectile() {
    GamePad& gamePad = Input::Instance().GetGamePad();

    // 直進弾丸発射
    if (gamePad.GetButtonDown() & GamePad::BTN_X) {
        // 前方向
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.x);
        dir.y = 0;
        dir.z = cosf(angle.z);
        // 発射位置 (プレイヤーの腰あたり)
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height * 0.5f;
        pos.z = position.z;
        // 発射
        ProjectileStraight* projectile = new ProjectileStraight (&projectileManager);
        projectile->Launch(dir, pos);
    }
}
