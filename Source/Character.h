#pragma once

#include <DirectXMath.h>
#include "Audio/Audio.h"
#include "Audio/AudioResource.h"
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Effect.h"

// �L�����N�^�[
class Character {
public:
    Character() { hitEffect = std::make_unique<Effect>("Data/Effect/Explosion.efk"),
        se_explosion = Audio::Instance().LoadAudioSource("Data/Audio/explosion05.wav");}
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

    // �g�����X�t�H�[���擾
    const DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

    // ���a�擾
    float GetRadius() const { return radius; }

    // �����擾
    float GetHeight() const { return height; }

    // �n�ʂɐڒn���Ă��邩
    bool IsGround() const { return isGround; }

    void CheckWallCollision();

    bool IsDead() const { return isDead; }

private:
    // �����ړ��X�V����
    void UpdateVerticalMove(float elapsedTime);

    // �����ړ��X�V����
    void UpdateHorizontalMove(float elapsedTime);

  

    void PlaySE();


protected:
    // �������͍X�V����
    virtual void UpdateVerticalVelocity(float elapsedFrame);

    // �������͍X�V�����@
    virtual void UpdateHorizontalVelocity(float elapsedFrame);

    // ���񏈗�
    void Turn(float elapsedTime, float vx, float vz, float speed);
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
    float invincibleTimer = 0.0f;
    float friction = 3.0f;      // ���C��
    float acceleration = 30.0f; // �����x
    float maxMoveSpeed = 50.0f; // �ő�����x
    float moveSpeed = 50.0f;
    float moveVecX = 0.0f;
    float moveVecY = 0.0f;
    float moveVecZ = 0.0f;
    float airControl = 0.3f;
    bool isDead = false;//���S����
    bool isStop = false;
    int timer = 0.0f;
    int counter = 1;
    int stopcounter = 1;
    std::unique_ptr<Model> model;
    std::unique_ptr<Effect> hitEffect = nullptr;
    std::unique_ptr<AudioSource>se_explosion = nullptr;


};