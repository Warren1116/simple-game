#include "EnemySlime.h"

// �R���X�g���N�^
EnemySlime::EnemySlime() {
    model = new Model("Data/Model/G/G.mdl");

    // ���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 5.0f;

    // ���A�����ݒ�
    radius = 10.0f;
    height = 5.0f;
}

// �f�X�g���N�^
EnemySlime::~EnemySlime() {
    delete model;
}

// �X�V����
void EnemySlime::Update(float elapsedTime) {
    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // �I�u�W�F�N�g�s����X�V
    UpdateTransform();

    // ���f���s��X�V
    if(model != nullptr) model->UpdateTransform(transform);
}

// �`�揈��
void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader) {
    if (model != nullptr) shader->Draw(dc, model);
}