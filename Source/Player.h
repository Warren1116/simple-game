#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"
#include "Graphics/Sprite.h"

// プレイヤー
class Player : public Character {
public:
    static Player& Instance() {
        static Player instance;
        return instance;
    }

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

    // 燃料取得
    float GetFuel() const { return fuel; }

    // 停止するか
    bool GetStop() const { return isStop; }

    // 死亡取得
    bool GetDead() const { return isDead; }

    // クリア判定取得
    bool GetClear() const { return ishitEnemy; }

protected:
    // 着地した時に呼ばれる
    void OnLanding() override;

private:
    void InputMove(float elapsedTime);
    DirectX::XMFLOAT3 GetMoveVec() const; // このconstは関数の中でメンバー変数の変更はしないと宣言しているモノ
    // プレイヤーとエネミーとの衝突処理
    void CollisionPlayerVsEnemies();
    // 前進処理
    void MoveFront(DirectX::XMFLOAT3 direction, float speed);
    // 回転処理
    void InputTurn(float elapsedTime, DirectX::XMFLOAT3 direction, float speed);
    // 速度変更処理
    void ChangeSpeed(float elapsedTime);
    // スピードがあるかどうか
    void ChackHasSpeed();
    // 燃料を使っているか
    void ChackUseFuel();
    // 重力による加減速
    void GravityAdjust(float elapsedFrame); // もう少しいい名前あったら変えたい

    void UpdateVerticalVelocity(float elapsedFrame) override;

    void UpdateHorizontalVelocity(float elapsedFrame) override;

    //クリア画面とゲームオーバー画面を描画する
    void DrawClear(ID3D11DeviceContext* dc);
    void DrawOver(ID3D11DeviceContext* dc);

private:
    //modelを消すためにスマートポインタを使う
    float turnSpeed = DirectX::XMConvertToRadians(100);
    float jumpSpeed = 20.0f;
    float deathSpeed = 0.0f; // 死んだとき使う

    float addSpeedEnergy = 5.0f; // 速度追加量
    float subSpeedEnergy = -1.0f; // 速度減少量
    float fuel = 100; // 燃料変数
    float subFuelEnergy = 10; // 燃料減少量

    int jumpCount = 0;
    int jumpLimit = 2;

    //死んだあと３秒間を数える
    int deathTimer = 180; //3秒間をCOUNTする
    bool ishitEnemy = false;


    int effectcount = 1;

    bool fuelUse = false; // 燃料使用しているかどうか
    bool hasSpeed = false; // 速度があるかどうか
    bool onSubSpeed = false; // 減速させるかどうか
    bool onAddSpeed = false; // 加速させるかどうか

    ProjectileManager projectileManager;
    //std::unique_ptr<Model> model;

    //effectの追加
    //std::unique_ptr<Effect> hitEffect = nullptr;
    std::unique_ptr<Effect> flyEffect = nullptr;
    Sprite* spriteGameclear = nullptr;
    Sprite* spriteGameover = nullptr;

private:
    // 定数 ImGuiで変更する都合上constがないですがここに書いている変数は定数つもりです

    // Y軸回転制限
    float AngleMaxY = DirectX::XMConvertToRadians(70);  // 右回転制限
    float AngleMinY = DirectX::XMConvertToRadians(-70); // 左回転制限
    // X軸回転制限
    float AngleMaxX = DirectX::XMConvertToRadians(160); // 下回転制限
    float AngleMinX = DirectX::XMConvertToRadians(20);  // 上回転制限
    // 速力がないときに向く角度
    float DyingAngleX = DirectX::XMConvertToRadians(120);

    float MaxAcceleSpeed = 60.0f;
    float MinAcceleSpeed = 0.0f;
    float MinVelocityZ = 20.0f;

private:
    // 補正値変数 初期値は変えないでください変更する際はImGui等でお願いします
    float DeathFallAdjustment = 0.005f;             // 死亡時の落下速度に加える補正値
    float DeathRotateAdjustment = 0.1f;             // 死亡時の回転速度に加える補正値
    float GravityYAdjustmentNormal = 0.1f;          // 通常時の、上方向に付与する重力に加える補正値
    float GravityYAdjustmentMinVelocityZ = 0.3f;    // velocity.zが最低速度時の、Y方向に付与する重力に加える補正値
    float GravityZAdjustmentAdd = 0.1f;             // 下向き時の、前方向に付与する重力に加える補正値
    float GravityZAdjustmentSub = 0.1f;             // 上向き時の、前方向に付与する重力に加える補正値
};