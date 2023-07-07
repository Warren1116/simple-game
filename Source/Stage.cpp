#include "Stage.h"

static Stage* instance = nullptr;

// コンストラクタ
Stage::Stage() {
    // ステージモデルを読み込み
	model = new Model("Data/Model/Stage/Stage_kari.mdl");
    instance = this;

	scale.x = scale.y = scale.z = 1.0f;
}

Stage::~Stage() {
    // ステージモデルを破棄
    delete model;
}

// 行列更新処理
void Stage::UpdateTransform()
{
	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	// ３つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	// 計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}


// 更新処理
void Stage::Update(float elapsedTime) {
    // 今は特にやることは無い
	UpdateTransform();

	model->UpdateTransform(transform);
}

// 描画処理
void Stage::Render(ID3D11DeviceContext* dc, Shader* shader) {
    // シェーダーにモデルを描画してもらう
    shader->Draw(dc, model);
}

// レイキャスト
bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}

Stage& Stage::Instance()
{
    return *instance;
}