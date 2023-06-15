#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"

// �v���C���[
class Player : public Character {
public:
    Player();
    ~Player() override;

    // �X�V����
    void Update(float elapsedTime);
    
    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // �f�o�b�O�pGUI�`��
    void DrawDebugGUI();

protected:
    // ���n�������ɌĂ΂��
    void OnLanding() override;
private:
    void InputMove(float elapsedTime);
    DirectX::XMFLOAT3 GetMoveVec() const; // ����const�͊֐��̒��Ń����o�[�ϐ��̕ύX�͂��Ȃ��Ɛ錾���Ă��郂�m
    // �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
    void CollisionPlayerVsEnemies();
    // �W�����v���͏���
    void InputJump();
    // �e�ۓ��͏���
    void InputProjectile();
    // �����������Ȃ�
    void UpdateVelocity(float elapsedTIme) override;
    
private:
    Model* model = nullptr;
    float moveSpeed = 5.0f;
    float turnSpeed = DirectX::XMConvertToRadians(720);
    float jumpSpeed = 20.0f;
    int jumpCount = 0;
    int jumpLimit = 2;
    ProjectileManager projectileManager;
};