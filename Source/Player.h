#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"
#include "Graphics/Sprite.h"

// �v���C���[
class Player : public Character {
public:
    static Player& Instance() {
        static Player instance;
        return instance;
    }

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

    // �R���擾
    float GetFuel() const { return fuel; }

    // ��~���邩
    bool GetStop() const { return isStop; }

    // ���S�擾
    bool GetDead() const { return isDead; }

    // �N���A����擾
    bool GetClear() const { return ishitEnemy; }

protected:
    // ���n�������ɌĂ΂��
    void OnLanding() override;

private:
    void InputMove(float elapsedTime);
    DirectX::XMFLOAT3 GetMoveVec() const; // ����const�͊֐��̒��Ń����o�[�ϐ��̕ύX�͂��Ȃ��Ɛ錾���Ă��郂�m
    // �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
    void CollisionPlayerVsEnemies();
    // �O�i����
    void MoveFront(DirectX::XMFLOAT3 direction, float speed);
    // ��]����
    void InputTurn(float elapsedTime, DirectX::XMFLOAT3 direction, float speed);
    // ���x�ύX����
    void ChangeSpeed(float elapsedTime);
    // �X�s�[�h�����邩�ǂ���
    void ChackHasSpeed();
    // �R�����g���Ă��邩
    void ChackUseFuel();
    // �d�͂ɂ�������
    void GravityAdjust(float elapsedFrame); // ���������������O��������ς�����

    void UpdateVerticalVelocity(float elapsedFrame) override;

    void UpdateHorizontalVelocity(float elapsedFrame) override;

    //�N���A��ʂƃQ�[���I�[�o�[��ʂ�`�悷��
    void DrawClear(ID3D11DeviceContext* dc);
    void DrawOver(ID3D11DeviceContext* dc);

private:
    //model���������߂ɃX�}�[�g�|�C���^���g��
    float turnSpeed = DirectX::XMConvertToRadians(100);
    float jumpSpeed = 20.0f;
    float deathSpeed = 0.0f; // ���񂾂Ƃ��g��

    float addSpeedEnergy = 5.0f; // ���x�ǉ���
    float subSpeedEnergy = -1.0f; // ���x������
    float fuel = 100; // �R���ϐ�
    float subFuelEnergy = 10; // �R��������

    int jumpCount = 0;
    int jumpLimit = 2;

    //���񂾂��ƂR�b�Ԃ𐔂���
    int deathTimer = 180; //3�b�Ԃ�COUNT����
    bool ishitEnemy = false;


    int effectcount = 1;

    bool fuelUse = false; // �R���g�p���Ă��邩�ǂ���
    bool hasSpeed = false; // ���x�����邩�ǂ���
    bool onSubSpeed = false; // ���������邩�ǂ���
    bool onAddSpeed = false; // ���������邩�ǂ���

    ProjectileManager projectileManager;
    //std::unique_ptr<Model> model;

    //effect�̒ǉ�
    //std::unique_ptr<Effect> hitEffect = nullptr;
    std::unique_ptr<Effect> flyEffect = nullptr;
    Sprite* spriteGameclear = nullptr;
    Sprite* spriteGameover = nullptr;

private:
    // �萔 ImGui�ŕύX����s����const���Ȃ��ł��������ɏ����Ă���ϐ��͒萔����ł�

    // Y����]����
    float AngleMaxY = DirectX::XMConvertToRadians(70);  // �E��]����
    float AngleMinY = DirectX::XMConvertToRadians(-70); // ����]����
    // X����]����
    float AngleMaxX = DirectX::XMConvertToRadians(160); // ����]����
    float AngleMinX = DirectX::XMConvertToRadians(20);  // ���]����
    // ���͂��Ȃ��Ƃ��Ɍ����p�x
    float DyingAngleX = DirectX::XMConvertToRadians(120);

    float MaxAcceleSpeed = 60.0f;
    float MinAcceleSpeed = 0.0f;
    float MinVelocityZ = 20.0f;

private:
    // �␳�l�ϐ� �����l�͕ς��Ȃ��ł��������ύX����ۂ�ImGui���ł��肢���܂�
    float DeathFallAdjustment = 0.005f;             // ���S���̗������x�ɉ�����␳�l
    float DeathRotateAdjustment = 0.1f;             // ���S���̉�]���x�ɉ�����␳�l
    float GravityYAdjustmentNormal = 0.1f;          // �ʏ펞�́A������ɕt�^����d�͂ɉ�����␳�l
    float GravityYAdjustmentMinVelocityZ = 0.3f;    // velocity.z���Œᑬ�x���́AY�����ɕt�^����d�͂ɉ�����␳�l
    float GravityZAdjustmentAdd = 0.1f;             // ���������́A�O�����ɕt�^����d�͂ɉ�����␳�l
    float GravityZAdjustmentSub = 0.1f;             // ��������́A�O�����ɕt�^����d�͂ɉ�����␳�l
};