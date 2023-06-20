#pragma once
#include <vector>
#include "Enemy.h"

// �G�l�~�[�}�l�[�W���[
class EnemyManager {
private:
    EnemyManager() {}
    ~EnemyManager() {}
public:
    // �B��̃C���X�^���X�擾
    static EnemyManager& Instance() {
        static EnemyManager instance;
        return instance;
    }

    // �G�l�~�[���擾
    int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

    // �G�l�~�[�擾
    Enemy* GetEnemy(int index) { return enemies.at(index); }

    // �G�l�~�[�o�^
    void Register(Enemy* enemy);

    // �X�V����
    void Update(float elapsedTime);

    // �`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    // �f�o�b�N�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // �G�l�~�[�S�폜
    void Clear();

private:
    // �G�l�~�[���m�̏Փˏ���
    void CollisionEnemyVsEnemies();
private:
    std::vector<Enemy*> enemies;
};