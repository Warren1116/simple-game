#pragma once

#include <DirectXMath.h>

// ƒRƒŠƒWƒ‡ƒ“
class Collision {
public:
    // ‹…‚Æ‹…‚ÌŒğ·”»’è
    static bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        DirectX::XMFLOAT3& outPositionB
    );

    // ‰~’Œ‚Æ‰~’Œ‚ÌŒğ·”»’è
    static bool IntersectCylinderVsCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB
    );
};