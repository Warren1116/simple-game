#include "Enemy.h"
#include "Graphics/Graphics.h"

// デバッグプリミティブ描画
void Enemy::DrawDebugPrimitive() {
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // 衝突判定用のデバック球を描画
    debugRenderer->DrawCylinder(position, radius,height, DirectX::XMFLOAT4(0, 0, 0, 1));
}