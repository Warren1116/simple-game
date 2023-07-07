#pragma once

#include <DirectXMath.h>
#include "Audio/Audio.h"
#include "Audio/AudioResource.h"

// キャラクター
class Character {
public:
    Character() { se_explosion = Audio::Instance().LoadAudioSource("Data/Audio/explosion05.wav"); }
    virtual ~Character(){}

    // 行列更新処理
    void UpdateTransform();

    // 位置取得
    const DirectX::XMFLOAT3& GetPosition() const { return position; }

    // 位置設定
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    // 回転取得
    const DirectX::XMFLOAT3& GetAngle() const { return angle; }

    // 回転設定
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

    // スケール取得
    const DirectX::XMFLOAT3& GetScale() const { return scale; }

    // スケール設定
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    // トランスフォーム取得
    const DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

    // 半径取得
    float GetRadius() const { return radius; }

    // 高さ取得
    float GetHeight() const { return height; }

    // 地面に接地しているか
    bool IsGround() const { return isGround; }

private:
    // 垂直速力更新処理　
    void UpdateVerticalVelocity(float elapsedFrame);

    // 垂直移動更新処理
    void UpdateVerticalMove(float elapsedTime);

    // 水平速力更新処理　
    void UpdateHorizontalVelocity(float elapsedFrame);

    // 水平移動更新処理
    void UpdateHorizontalMove(float elapsedTime);

protected:
    // 移動処理
    //void Move(float vx, float vz, float speed);
    // 旋回処理
    void Turn(float elapsedTime, float vx, float vz, float speed);
    // ジャンプ処理
    //void Jump(float speed);
    // 速度処理更新
    virtual void UpdateVelocity(float elasepdTime);
    // 着地した時に呼ばれる
    virtual void OnLanding() {}
    bool IsDead() const { return isDead; }


protected:
    DirectX::XMFLOAT3 position = { 0,0,0 };
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    DirectX::XMFLOAT3 scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    std::unique_ptr<AudioSource>se_explosion = nullptr;
    float radius = 0.5f; // 半径50cmということ
    float gravity = -1.0f;
    float height = 2.0f;
    DirectX::XMFLOAT3 velocity = { 0,0,0 };
    bool isGround = false;
    float stepOffset = 1.0f;
    float invincibleTimer = 0.0f;
    float friction = 0.5f;
    float acceleration = 1.0f;
    float maxMoveSpeed = 5.0f;
    float moveVecX = 0.0f;
    float moveVecY = 0.0f;
    float moveVecZ = 0.0f;
    float airControl = 0.3f;
    bool isDead = false;//死亡判定
    int timer = 0.0f;

};