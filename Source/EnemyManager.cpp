#include "EnemyManager.h"
#include "Collision.h"

// �X�V����
void EnemyManager::Update(float elapsedTime) {
    for (Enemy* enemy : enemies) {
        enemy->Update(elapsedTime);
    }
    CollisionEnemyVsEnemies();
}

// �`�揈��
void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader) {
    for (Enemy* enemy : enemies) {
        enemy->Render(context, shader);
    }
}

// �G�l�~�[�o�^
void EnemyManager::Register(Enemy* enemy) {
    enemies.emplace_back(enemy);
}

void EnemyManager::DrawDebugPrimitive() {
    for (Enemy* enemy : enemies) {
        enemy->DrawDebugPrimitive();
    }
}

// �G�l�~�[�S�폜
void EnemyManager::Clear() {
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
}

// �G�l�~�[���m�̏Փˏ���
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