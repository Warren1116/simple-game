#include "Stage.h"

static Stage* instance = nullptr;

// �R���X�g���N�^
Stage::Stage() {
    // �X�e�[�W���f����ǂݍ���
    model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
    instance = this;
}

Stage::~Stage() {
    // �X�e�[�W���f����j��
    delete model;
}

// �X�V����
void Stage::Update(float elapsedTime) {
    // ���͓��ɂ�邱�Ƃ͖���
}

// �`�揈��
void Stage::Render(ID3D11DeviceContext* dc, Shader* shader) {
    // �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
    shader->Draw(dc, model);
}

// ���C�L���X�g
bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}

Stage& Stage::Instance()
{
    return *instance;
}