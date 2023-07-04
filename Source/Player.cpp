#include <imgui.h>
#include "Player.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ProjectileStraight.h"
#include "Collision.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include <cmath>

// コンストラクタ
Player::Player() {
    model = new Model("Data/Model/Player/Missile.mdl");

    // モデルが大きいのでスケーリング
    scale.x = scale.y = scale.z = 0.08f;
    position.y = 1.0f;
    angle.x = DirectX::XMConvertToRadians(90);

    hitEffect = new Effect("Data/Effect/Explosion.efk");
    flyEffect = new Effect("Data/Effect/Explosion.efk");

}

// デストラクタ
Player::~Player() {
    delete hitEffect;
    delete flyEffect;
    delete model;
}

// 更新処理
void Player::Update(float elapsedTime) {
    InputMove(elapsedTime);

    // ジャンプ入力処理
    //InputJump();

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
    if (model != nullptr)
    {
        model->UpdateTransform(transform);
    }
}

// 描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader) {
    if (model != nullptr)
    {
        shader->Draw(dc, model);

        // 弾丸描画処理
        projectileManager.Render(dc,shader);

    }
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

// スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const {
    // 本当は前方ベクトルを取り出すが90度回転させているので上方向ベクトルを入れておく
    DirectX::XMFLOAT3 vec = { transform._21, transform._22, transform._23 };

    return vec;
}

// 移動入力処理
void Player::InputMove(float elapsedTime) {
    // 進行ベクトル取得
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    // 燃料を使っているか
    ChackUseFuel();

    // 速度があるか
    ChackHasSpeed();

    // 速度調整処理
    ChackMoveSpeed(elapsedTime);

    // 移動処理
    MoveFront(moveVec, moveSpeed);

    // 入力回転処理
    if (hasSpeed) InputTurn(elapsedTime, moveVec, turnSpeed);
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
            }
            // 押し出し後の位置設定
            else enemy->SetPosition(outPosition);
            hitEffect->Play(this->GetPosition());
            model = nullptr;
            
        }
    }
}

// ジャンプ入力処理
//void Player::InputJump() {
//    // ボタン入力でジャンプ(ジャンプ回数制限つき)
//    GamePad& gamePad = Input::Instance().GetGamePad();
//    if (gamePad.GetButtonDown() & GamePad::BTN_A) {
//        if (jumpCount < jumpLimit) {
//            Jump(jumpSpeed);
//            jumpCount++;
//        }
//    }
//}

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

// 前進処理
void Player::MoveFront(DirectX::XMFLOAT3 direction, float speed) {
    // 常に前進処理

    moveVecX += direction.x;
    moveVecY += direction.y;
    moveVecZ += direction.z;

    maxMoveSpeed = speed;
    //flyEffect->Play(this->GetPosition());
}

// 向き変更
void Player::InputTurn(float elapsedTime, DirectX::XMFLOAT3 direction, float speed) {
    speed *= elapsedTime;

    // 入力情報を取得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    //// 内積値は-1.0～1.0で表現されており、2つの単位ベクトルの角度が
    //// 小さいほど1.0に近づくという性質を利用して回転速度を調整する
    //float rot = 1.0 - dot; // 補正値
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