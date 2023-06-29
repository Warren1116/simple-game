#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"

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
    // 前進処理
    void MoveFront(float elapsedTime, DirectX::XMFLOAT3 direction, float speed);
    // 回転処理
    void InputTurn(float elapsedTime, DirectX::XMFLOAT3 direction, float speed);
    // 燃料処理
    void ChackMoveSpeed(float elapsedTime); // こいつの中で燃料関連の処理をする
    // スピードがあるかどうか
    void ChackHasSpeed();
    // 燃料を使っているか
    void ChackUseFuel();
private:
    Model* model = nullptr;
    float moveSpeed = 80.0f;
    float turnSpeed = DirectX::XMConvertToRadians(300);
    float jumpSpeed = 20.0f;

    float addSpeedEnergy = 100.0f; // 速度追加量
    float subSpeedEnergy = 10.0f; // 速度減少量
    float fuel = 100; // 燃料変数
    float subFuelEnergy = 10; // 燃料減少量

    int jumpCount = 0;
    int jumpLimit = 2;

    bool fuelUse = false; // 燃料使用しているかどうか
    bool hasSpeed = false; // 速度があるかどうか

    ProjectileManager projectileManager;
    Effect* hitEffect = nullptr;
private:
    // Y軸回転制限
    const float AngleMaxY = DirectX::XMConvertToRadians(35);  // 右回転制限
    const float AngleMinY = DirectX::XMConvertToRadians(-35); // 左回転制限
    // X軸回転制限
    const float AngleMaxX = DirectX::XMConvertToRadians(115); // 下回転制限
    const float AngleMinX = DirectX::XMConvertToRadians(65);  // 上回転制限
};