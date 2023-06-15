#include "Collision.h"

// ���Ƌ��̌�������
bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA, // �ʒuA
    float radiusA,                      // ���aA
    const DirectX::XMFLOAT3& positionB, // �ʒuB
    float radiusB,                      // ���aB
    DirectX::XMFLOAT3& outPositionB) {
    //  A��B�̒P�ʃx�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec); // Length��Sq��t����Ɓ�̂܂܂ɂł���㏈�����Ȃ��ق����y���̂�Sq���g���Ă���
    float lengthSq; // �x�N�g���̒���
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    // ��������
    float range = radiusA + radiusB;
    if (lengthSq > range * range) {
        return false;
    }

    // A��B�������o��
    Vec = DirectX::XMVector3Normalize(Vec); // �P�ʃx�N�g����
    Vec = DirectX::XMVectorScale(Vec, range); // range��Vec�𒷂�����
    PositionB = DirectX::XMVectorAdd(PositionA,Vec); // B�̈ʒu��PositionA����Vec���������l��ݒ�
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
    // A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (positionA.y > positionB.y + heightB) {
        return false;
    }
    // A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (positionA.y + heightA < positionB.y) {
        return false;
    }
    // XZ���ʂł͈̔̓`�F�b�N
    float vecX = positionB.x - positionA.x;
    float vecZ = positionB.z - positionA.z;
    float length = sqrtf(vecX * vecX + vecZ * vecZ);
    float range = radiusA + radiusB;
    if (length > range) {
        return false;
    }
    // A��B�������o��
    vecX /= length; // ���K��
    vecZ /= length; // ���K��

    outPositionB.x = positionB.x + vecX * (range - length);
    outPositionB.y = positionB.y;
    outPositionB.z = positionB.z + vecZ * (range - length);

    return true;
}