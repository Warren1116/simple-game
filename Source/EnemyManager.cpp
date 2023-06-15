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

    // 全ての敵と総当たりで衝突処理
    //int enemyCount = enemyManager.GetEnemyCount();
    // 2重ループ
    //for (int i = 0; i < enemyCount; i++) {
    //    Enemy* enemy = enemyManager.GetEnemy(i);
    //    for (int j = 0; j < enemyCount; j++) {
    //        Enemy* enemies = enemyManager.GetEnemy(j);
    //        // 衝突処理
    //        DirectX::XMFLOAT3 outPosition;
    //        if (Collision::IntersectSphereVsSphere(
    //            enemy->GetPosition(), enemy->GetRadius(),
    //            enemies->GetPosition(), enemies->GetRadius(),
    //            outPosition
    //        )) {
    //            // 押し出し後の位置設定
    //            enemies->SetPosition(outPosition);
    //        }
    //    }
    //}
    
    // 先生の答え このやり方だと最後のやつは絶対に押し返すことは出来ない
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