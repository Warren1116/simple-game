#include "EnemySlime.h"

// コンストラクタ
EnemySlime::EnemySlime() {
    model = new Model("Data/Model/G/G.mdl");

    // モデルが大きいのでスケーリング
    scale.x = scale.y = scale.z = 1.0f;

    // 幅、高さ設定
    radius = 4.0f; // 2.0f　← 元の大きさ
    height = 3.0f; // 1.0f    元の大きさ
}

// デストラクタ
EnemySlime::~EnemySlime() {
    delete model;
}

// 更新処理
void EnemySlime::Update(float elapsedTime) {
    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // オブジェクト行列を更新
    UpdateTransform();

    // モデル行列更新
    if(model != nullptr) model->UpdateTransform(transform);
}

// 描画処理
void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader) {
    if (model != nullptr) shader->Draw(dc, model);
}