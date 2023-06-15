#include "Character.h"

// 行列更新処理
void Character::UpdateTransform() {
    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    // 回転行列を作成
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    // 位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    // 3つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;
    // 計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}

// 移動処理
void Character::Move(float elapsedTime, float vx, float vz, float speed) {
    speed *= elapsedTime;
    position.x += vx * speed;
    position.z += vz * speed;
}

// 旋回処理
void Character::Turn(float elapsedTime, float vx, float vz, float speed) {
    speed *= elapsedTime;

    // 進行ベクトルがゼロベクトルの場合は処理する必要なし
    float length = sqrtf(vx * vx + vz * vz);
    if (length < 0.001f) return;

    // 進行ベクトルを単位ベクトル比
    vx /= length;
    vz /= length;

    // 自身の回転値から前方向を求める
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    // 回転角を求めるため、2つの単位ベクトルの内積を計算する
    float dot = (vx * frontX) + (vz * frontZ);

    // 内積値は-1.0～1.0で表現されており、2つの単位ベクトルの角度が
    // 小さいほど1.0に近づくという性質を利用して回転速度を調整する
    float rot = 1.0 - dot; // 補正値
    if (rot > speed) rot = speed;

    // 左右判定を行うために2つの単位ベクトルの外積を計算する
    float cross = (frontZ * vx) - (frontX * vz);

    // 2Dの外積値が正の場合か負の場合によって左右判定が行える
    // 左右判定を行うことによって左右判定を選択する
    if (cross < 0.0f) {
        angle.y -= rot;
    }
    else {
        angle.y += rot;
    }
}

// ジャンプ処理
void Character::Jump(float speed) {
    // 上方向の力を設定
    velocity.y += speed;
}

// 速力処理更新
void Character::UpdateVelocity(float elapsedTime) {
    // 経過フレーム
    float elapsedFrame = 60.0f * elapsedTime; // 60 * 0.0167 = 1

    // 重力処理
    velocity.y += gravity * elapsedFrame;

    // 移動処理
    position.y += velocity.y * elapsedTime;

    // 地面設定
    if (position.y < 0.0f) {
        position.y = 0.0f;
        velocity.y = 0.0f;

        // 着地した
        isGround = true;
        if (isGround) {
            OnLanding();
        }
    }
    else {
        isGround = false;
    }
}