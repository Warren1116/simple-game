#include "Enemy.h"
#include "Graphics/Graphics.h"

// �f�o�b�O�v���~�e�B�u�`��
void Enemy::DrawDebugPrimitive() {
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    // �Փ˔���p�̃f�o�b�N����`��
    debugRenderer->DrawCylinder(position, radius,height, DirectX::XMFLOAT4(0, 0, 0, 1));
}