#include "EnemyManager.h"
#include "Collision.h"

// 更新処理
void EnemyManager::Update(float elapsedTime) {
    for (Enemy* enemy : enemies) {
        enemy->Update(elapsedTime);
    }
    CollisionEnemyVsEnemies();
}

// 描画処理
void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader) {
    for (Enemy* enemy : enemies) {
        enemy->Render(context, shader);
    }
}

// エネミー登録
void EnemyManager::Register(Enemy* enemy) {
    enemies.emplace_back(enemy);
}

void EnemyManager::DrawDebugPrimitive() {
    for (Enemy* enemy : enemies) {
        enemy->DrawDebugPrimitive();
    }
}

// エネミー全削除
void EnemyManager::Clear() {
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
}

// エネミー同士の衝突処理
void EnemyManager::CollisionEnemyVsEnemies() {
    EnemyManager& enemyManager = EnemyManager::Instance();

    size_t enemyCount = enemies.size();
    for (int i = 0; i < enemyCount; ++i) {
        Enemy* enemyA = enemies.at(i);
        for (int j = i + 1; j < enemyCount; ++j) {
            Enemy* enemyB = enemies.at(j);
            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectCylinderVsCylinder(
                enemyA->GetPosition(),
                enemyA->GetRadius(),
                enemyA->GetHeight(),
                enemyB->GetPosition(),
                enemyB->GetRadius(),
                enemyB->GetHeight(),
                outPosition
            )) {
                enemyB->SetPosition(outPosition);
            }
        }
    }
}