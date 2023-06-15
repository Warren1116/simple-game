#include "Character.h"

// �s��X�V����
void Character::UpdateTransform() {
    // �X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    // ��]�s����쐬
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    // �ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    // 3�̍s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    // �v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&transform, W);
}

// �ړ�����
void Character::Move(float elapsedTime, float vx, float vz, float speed) {
    speed *= elapsedTime;
    position.x += vx * speed;
    position.z += vz * speed;
}

// ���񏈗�
void Character::Turn(float elapsedTime, float vx, float vz, float speed) {
    speed *= elapsedTime;

    // �i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
    float length = sqrtf(vx * vx + vz * vz);
    if (length < 0.001f) return;

    // �i�s�x�N�g����P�ʃx�N�g����
    vx /= length;
    vz /= length;

    // ���g�̉�]�l����O���������߂�
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    // ��]�p�����߂邽�߁A2�̒P�ʃx�N�g���̓��ς��v�Z����
    float dot = (vx * frontX) + (vz * frontZ);

    // ���ϒl��-1.0�`1.0�ŕ\������Ă���A2�̒P�ʃx�N�g���̊p�x��
    // �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
    float rot = 1.0 - dot; // �␳�l
    if (rot > speed) rot = speed;

    // ���E������s�����߂�2�̒P�ʃx�N�g���̊O�ς��v�Z����
    float cross = (frontZ * vx) - (frontX * vz);

    // 2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
    // ���E������s�����Ƃɂ���č��E�����I������
    if (cross < 0.0f) {
        angle.y -= rot;
    }
    else {
        angle.y += rot;
    }
}

// �W�����v����
void Character::Jump(float speed) {
    // ������̗͂�ݒ�
    velocity.y += speed;
}

// ���͏����X�V
void Character::UpdateVelocity(float elapsedTime) {
    // �o�߃t���[��
    float elapsedFrame = 60.0f * elapsedTime; // 60 * 0.0167 = 1

    // �d�͏���
    velocity.y += gravity * elapsedFrame;

    // �ړ�����
    position.y += velocity.y * elapsedTime;

    // �n�ʐݒ�
    if (position.y < 0.0f) {
        position.y = 0.0f;
        velocity.y = 0.0f;

        // ���n����
        isGround = true;
        if (isGround) {
            OnLanding();
        }
    }
    else {
        isGround = false;
    }
}