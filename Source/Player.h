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

    Model* GetModel() { return model.get(); }


protected:
    // ���n�������ɌĂ΂��
    void OnLanding() override;
    std::unique_ptr<Model> model;

private:
    void InputMove(float elapsedTime);
    DirectX::XMFLOAT3 GetMoveVec() const; // ����const�͊֐��̒��Ń����o�[�ϐ��̕ύX�͂��Ȃ��Ɛ錾���Ă��郂�m
    // �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
    void CollisionPlayerVsEnemies();
    // �O�i����
    void MoveFront(DirectX::XMFLOAT3 direction, float speed);
    // ��]����
    void InputTurn(float elapsedTime, DirectX::XMFLOAT3 direction, float speed);
    // �R������
    void ChackMoveSpeed(float elapsedTime); // �����̒��ŔR���֘A�̏���������
    // �X�s�[�h�����邩�ǂ���
    void ChackHasSpeed();
    // �R�����g���Ă��邩
    void ChackUseFuel();

    //�N���A��ʂƃQ�[���I�[�o�[��ʂ�`�悷��
    void DrawClear(ID3D11DeviceContext* dc);
    void DrawOver(ID3D11DeviceContext* dc);



private:
    //model���������߂ɃX�}�[�g�|�C���^���g��
    float moveSpeed = 800.0f;
    float turnSpeed = DirectX::XMConvertToRadians(100);
    float jumpSpeed = 20.0f;

    float addSpeedEnergy = 3.0f; // ���x�ǉ���
    float subSpeedEnergy = 10.0f; // ���x������
    float fuel = 100; // �R���ϐ�
    float subFuelEnergy = 10; // �R��������

    int jumpCount = 0;
    int jumpLimit = 2;

    //���񂾂��ƂR�b�Ԃ𐔂���
    int timer = 180; //3�b�Ԃ�COUNT����
    bool ishitEnemy = false;


    int effectcount = 1;

    bool fuelUse = false; // �R���g�p���Ă��邩�ǂ���
    bool hasSpeed = false; // ���x�����邩�ǂ���

    ProjectileManager projectileManager;

    //effect�̒ǉ�
    std::unique_ptr<Effect> hitEffect = nullptr;
    std::unique_ptr<Effect> flyEffect = nullptr;
    Sprite* spriteGameclear = nullptr;
    Sprite* spriteGameover = nullptr;
    Effekseer::Handle handle = 0;

private:
    // Y����]����
    const float AngleMaxY = DirectX::XMConvertToRadians(90);  // �E��]����
    const float AngleMinY = DirectX::XMConvertToRadians(-90); // ����]����
    // X����]����
    const float AngleMaxX = DirectX::XMConvertToRadians(115); // ����]����
    const float AngleMinX = DirectX::XMConvertToRadians(65);  // ���]����
};