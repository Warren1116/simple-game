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


// コンストラクタ
Player::Player() {
    model = std::make_unique<Model>("Data/Model/Player/Rocket.mdl");
    // モデルが大きいのでスケーリング
    scale.x = scale.y = scale.z = 0.05f;
    position.y = 60.0f;
    position.z = -600.0f;
    angle.x = DirectX::XMConvertToRadians(90);

    hitEffect = std::make_unique<Effect>("Data/Effect/Explosion.efk");
    flyEffect = std::make_unique<Effect>("Data/Effect/engine0.1.efk");

    spriteGameclear = new Sprite("Data/Sprite/gameclear.png");
    spriteGameover = new Sprite("Data/Sprite/gameover.png");

}

// デストラクタ
Player::~Player() {
    delete spriteGameover;
    delete spriteGameclear;

}

// 更新処理
void Player::Update(float elapsedTime) {
    InputMove(elapsedTime);

    // 速力更新処理
    UpdateVelocity(elapsedTime);

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

            // なにかボタンを押したらローディングシーンへ切り替え
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

            // なにかボタンを押したらローディングシーンへ切り替え
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

// 着地した時に呼ばれる
void Player::OnLanding() {
    jumpCount = 0;
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
