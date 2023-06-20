#pragma once

#include <DirectXMath.h>

// �L�����N�^�[
class Character {
public:
    Character(){}
    virtual ~Character(){}

    // �s��X�V����
    void UpdateTransform();

    // �ʒu�擾
    const DirectX::XMFLOAT3& GetPosition() const { return position; }

    // �ʒu�ݒ�
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    // ��]�擾
    const DirectX::XMFLOAT3& GetAngle() const { return angle; }

    // ��]�ݒ�
    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

    // �X�P�[���擾
    const DirectX::XMFLOAT3& GetScale() const { return scale; }

    // �X�P�[���ݒ�
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    // ���a�擾
    float GetRadius() const { return radius; }

    // �����擾
    float GetHeight() const { return height; }

    // �n�ʂɐڒn���Ă��邩
    bool IsGround() const { return isGround; }

private:
    // �������͍X�V�����@
    void UpdateVerticalVelocity(float elapsedFrame);

    // �����ړ��X�V����
    void UpdateVerticalMove(float elapsedTime);

protected:
    // �ړ�����
    void Move(float elapsedTime, float vx, float vz, float speed);
    // ���񏈗�
    void Turn(float elapsedTime, float vx, float vz, float speed);
    // �W�����v����
    void Jump(float speed);
    // ���x�����X�V
    virtual void UpdateVelocity(float elasepdTime);
    // ���n�������ɌĂ΂��
    virtual void OnLanding() {}
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
    float radius = 0.5f; // ���a50cm�Ƃ�������
    float gravity = -1.0f;
    float height = 2.0f;
    DirectX::XMFLOAT3 velocity = { 0,0,0 };
    bool isGround = false;
    float stepOffset = 1.0f;
};