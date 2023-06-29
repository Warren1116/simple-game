#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"

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
    // �O�i����
    void MoveFront(float elapsedTime, DirectX::XMFLOAT3 direction, float speed);
    // ��]����
    void InputTurn(float elapsedTime, DirectX::XMFLOAT3 direction, float speed);
    // �R������
    void ChackMoveSpeed(float elapsedTime); // �����̒��ŔR���֘A�̏���������
    // �X�s�[�h�����邩�ǂ���
    void ChackHasSpeed();
    // �R�����g���Ă��邩
    void ChackUseFuel();
private:
    Model* model = nullptr;
    float moveSpeed = 80.0f;
    float turnSpeed = DirectX::XMConvertToRadians(300);
    float jumpSpeed = 20.0f;

    float addSpeedEnergy = 100.0f; // ���x�ǉ���
    float subSpeedEnergy = 10.0f; // ���x������
    float fuel = 100; // �R���ϐ�
    float subFuelEnergy = 10; // �R��������

    int jumpCount = 0;
    int jumpLimit = 2;

    bool fuelUse = false; // �R���g�p���Ă��邩�ǂ���
    bool hasSpeed = false; // ���x�����邩�ǂ���

    ProjectileManager projectileManager;
    Effect* hitEffect = nullptr;
private:
    // Y����]����
    const float AngleMaxY = DirectX::XMConvertToRadians(35);  // �E��]����
    const float AngleMinY = DirectX::XMConvertToRadians(-35); // ����]����
    // X����]����
    const float AngleMaxX = DirectX::XMConvertToRadians(115); // ����]����
    const float AngleMinX = DirectX::XMConvertToRadians(65);  // ���]����
};