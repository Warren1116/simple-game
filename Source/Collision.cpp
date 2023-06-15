#include "Collision.h"

// 球と球の交差判定
bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA, // 位置A
    float radiusA,                      // 半径A
    const DirectX::XMFLOAT3& positionB, // 位置B
    float radiusB,                      // 半径B
    DirectX::XMFLOAT3& outPositionB) {
    //  A→Bの単位ベクトルを算出
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec); // LengthにSqを付けると√のままにできる√処理しないほうが軽いのでSqを使っている
    float lengthSq; // ベクトルの長さ
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    // 距離判定
    float range = radiusA + radiusB;
    if (lengthSq > range * range) {
        return false;
    }

    // AがBを押し出す
    Vec = DirectX::XMVector3Normalize(Vec); // 単位ベクトル化
    Vec = DirectX::XMVectorScale(Vec, range); // range分Vecを長くする
    PositionB = DirectX::XMVectorAdd(PositionA,Vec); // Bの位置をPositionAからVec分足した値を設定
    DirectX::XMStoreFloat3(&outPositionB, PositionB);
    return true;
}

bool Collision::IntersectCylinderVsCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outPositionB
) {
    // Aの足元がBの頭より上なら当たっていない
    if (positionA.y > positionB.y + heightB) {
        return false;
    }
    // Aの頭がBの足元より下なら当たっていない
    if (positionA.y + heightA < positionB.y) {
        return false;
    }
    // XZ平面での範囲チェック
    float vecX = positionB.x - positionA.x;
    float vecZ = positionB.z - positionA.z;
    float length = sqrtf(vecX * vecX + vecZ * vecZ);
    float range = radiusA + radiusB;
    if (length > range) {
        return false;
    }
    // AがBを押し出す
    vecX /= length; // 正規化
    vecZ /= length; // 正規化

    outPositionB.x = positionB.x + vecX * (range - length);
    outPositionB.y = positionB.y;
    outPositionB.z = positionB.z + vecZ * (range - length);

    return true;
}