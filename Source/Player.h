#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"

// プレイヤー
class Player : public Character {
public:
    Player();
    ~Player() override;

    // 更新処理
    void Update(float elapsedTime);
    
    // 描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // デバッグ用GUI描画
    void DrawDebugGUI();

protected:
    // 着地した時に呼ばれる
    void OnLanding() override;
private:
    void InputMove(float elapsedTime);
    DirectX::XMFLOAT3 GetMoveVec() const; // このconstは関数の中でメンバー変数の変更はしないと宣言しているモノ
    // プレイヤーとエネミーとの衝突処理
    void CollisionPlayerVsEnemies();
    // ジャンプ入力処理
    void InputJump();
    // 弾丸入力処理
    void InputProjectile();
    // 落下処理しない
    void UpdateVelocity(float elapsedTIme) override;
    
private:
    Model* model = nullptr;
    float moveSpeed = 5.0f;
    float turnSpeed = DirectX::XMConvertToRadians(720);
    float jumpSpeed = 20.0f;
    int jumpCount = 0;
    int jumpLimit = 2;
    ProjectileManager projectileManager;
};